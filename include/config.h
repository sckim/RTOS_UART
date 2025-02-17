#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

// DFU 설정
#define STACK_SIZE_DFU 1024
#define PRIORITY_DFU 1

// GPIO 설정
#define BUTTON_PIN 5
#define LED_PIN LED_BUILTIN
#define SHORT_PRESS_TIME 500 // 1초
#define LONG_PRESS_TIME 1500 // 1.5초

// 태스크 설정
#define STACK_SIZE_GPIO 1024
#define STACK_SIZE_BLE 4096
#define PRIORITY_GPIO_INPUT 2
#define PRIORITY_GPIO_OUTPUT 2
#define PRIORITY_BLE 1

// BLE 설정
#define UUID16_SVC_DATA_SERVICE 0x180F
#define UUID16_CHR_DATA_CHAR 0x2A19
#define BLE_MTU_SIZE 247
#define QUEUE_SIZE 10

// 버퍼 크기도 증가
#define MAX_CHUNK_SIZE 512   // BLE MTU 크기와 동일하게 설정
#define MAX_BUFFER_SIZE 2048 // 전체 버퍼 크기도 증가

// ADC 설정
#define ADC_PIN A0
#define STACK_SIZE_ADC 1024
#define PRIORITY_ADC 3
#define SAMPLING_TIME 100 // 100msec

typedef enum {
    cmdLED = 0x01,         // LED 제어 명령. LED의 상태를 제어합니다.
    cmdStatus = 0x02,      // 시스템 상태 요청. 현재 시스템 상태를 전송합니다.
    cmdLargeData = 0x03,   // 대용량 데이터 전송 요청. 테스트 데이터의 큰 버퍼를 전송합니다.
    cmdDiagnosis = 0x04,   // 시스템 진단 데이터 요청. 진단 정보를 전송합니다.
    cmdADC = 0x05,         // ADC
    cmdUnknown = 0xFF      // Unknown command
} BLECommand_t;

#define DEBUG 1
#if DEBUG
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

#endif // CONFIG_H