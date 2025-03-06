#include <FreeRTOS.h>
#include <semphr.h>
#include <queue.h>

#include "config.h"
#include "button_handler.h"
#include "led_handler.h"
#include "adc_handler.h"
#include "ble_handler.h"
#include "error_handler.h"

// 전역 객체 선언
ButtonHandler MainButton(BUTTON_PIN);
LEDHandler RedLedIndicator(LED_RED);
LEDHandler BlueLedIndicator(LED_BLUE);

BLEHandler bleHandler;
ADCHandler adcHandler(ADC_PIN);

// FreeRTOS 객체
SemaphoreHandle_t xGPIOSemaphore;
QueueHandle_t xButtonEventQueue = NULL;

uint8_t adcDataBuffer[MAX_BUFFER_SIZE];
size_t adcDataLength;

void TaskADC(void *pvParameters)
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    static size_t dataIndex = 0;

    DEBUG_PRINTF("ADC Task Started\n");
    pinMode(6, OUTPUT);

    for (;;)
    {
        if (xGPIOSemaphore != NULL && xSemaphoreTake(xGPIOSemaphore, pdMS_TO_TICKS(5)) == pdTRUE)
        {
            // int adcValue = adcHandler.readADC();
            int adcValue = dataIndex; // for testing

            adcDataBuffer[dataIndex * 2] = (adcValue >> 8) & 0xFF;
            adcDataBuffer[dataIndex * 2 + 1] = adcValue & 0xFF;
            dataIndex++;

            if (dataIndex >= 512)
            {
                if (bleHandler.getConnected())
                {
                    // BLE UART를 통해 직접 데이터 전송
                    uint8_t header[1] = {0x05}; // ADC 데이터 타입
                    bleHandler.sendData(header, 1);
                    bleHandler.sendData(adcDataBuffer, dataIndex * 2);
                }
                dataIndex = 0;
            }

            xSemaphoreGive(xGPIOSemaphore);
        }
        digitalWrite(6, !digitalRead(6));
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(SAMPLING_TIME));
    }
}

void TaskRedLedIndicator(void *pvParameters)
{
    ButtonEvent_t receivedEvent;

    for (;;)
    {
        if (xQueueReceive(xButtonEventQueue, &receivedEvent, portMAX_DELAY) == pdTRUE)
        {
            switch (receivedEvent)
            {
            case BUTTON_SHORT_PRESS:
                DEBUG_PRINTF("Short press\n");
                RedLedIndicator.shortBlink();
                break;
            case BUTTON_MEDIUM_PRESS:
                DEBUG_PRINTF("Medium press\n");
                RedLedIndicator.mediumBlink();
                break;
            case BUTTON_LONG_PRESS:
                DEBUG_PRINTF("Long press\n");
                RedLedIndicator.longBlink();
                break;
            case BUTTON_NO_EVENT:
                DEBUG_PRINTF("No event\n");
                break;
            }
        }
    }
}

void setup()
{
    Serial.begin(115200);

#if DEBUG && (defined(ARDUINO_NRF52840_ADAFRUIT) || \
              defined(ARDUINO_NRF52_ADAFRUIT) ||    \
              defined(ARDUINO_AVR_LEONARDO) ||      \
              defined(ARDUINO_SAMD_ZERO))
    while (!Serial)
        delay(10);
#endif
    DEBUG_PRINTF("Starting BLE Sensor Node\n");

    xButtonEventQueue = xQueueCreate(QUEUE_SIZE, sizeof(ButtonEvent_t));
    if (xButtonEventQueue == NULL)
    {
        errorHandler("Failed to create queue");
    }

    xGPIOSemaphore = xSemaphoreCreateMutex();
    if (xGPIOSemaphore == NULL)
    {
        errorHandler("Failed to create semaphore");
    }

    MainButton.begin();
    RedLedIndicator.turnOff();

    adcHandler.init();
    bleHandler.init();

    xTaskCreate(TaskRedLedIndicator, "GeneralOutput", STACK_SIZE_GPIO, NULL, PRIORITY_GPIO_OUTPUT, NULL);
}

void loop()
{
    // FreeRTOS 사용시 loop는 비워둠
}