#include "ble_handler.h"

BLEHandler *BLEHandler::instance = nullptr;

BLEHandler::BLEHandler() : isBLEConnected(false)
{
    instance = this;
}

void BLEHandler::init()
{
    Bluefruit.begin();
    Bluefruit.setName("HKNU EE RTOS");
    Bluefruit.setTxPower(4);

    // BLE UART 서비스 설정
    bleuart.begin();

    // 콜백 함수 설정
    Bluefruit.Periph.setConnectCallback(connect_callback);
    Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
    bleuart.setRxCallback(bleuart_rx_callback);

    startAdv();
}

void BLEHandler::startAdv(void)
{
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addTxPower();

    // BLE UART 서비스 광고에 추가
    Bluefruit.Advertising.addService(bleuart);

    Bluefruit.Advertising.addName();  // shortened name이 올 수 있다.
    Bluefruit.ScanResponse.addName(); // comlete name 강제

    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(32, 244);
    Bluefruit.Advertising.setFastTimeout(30);
    Bluefruit.Advertising.start(0);
}
void BLEHandler::connect_callback(uint16_t conn_handle)
{
    if (instance != nullptr)
    {
        DEBUG_PRINTF("Connected\n");

        instance->setConnected(true);
    }
}

void BLEHandler::disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
    if (instance != nullptr)
    {
        DEBUG_PRINTF("Disconnected. Reason(%d)\n", reason);
        instance->setConnected(false);
    }
}

void BLEHandler::bleuart_rx_callback(uint16_t conn_handle)
{
    if (instance != nullptr)
    {
        DEBUG_PRINTF("bleuart_rx_callback\n");
        instance->processCommand();
    }
}

void BLEHandler::sendLargeData(const uint8_t* data, size_t length) {
    if (!isBLEConnected || length > MAX_BUFFER_SIZE) {
        DEBUG_PRINTF("Data too large for buffer or not connected\n");
        return;
    }

    // MTU 크기 설정 (헤더 1바이트를 고려)
    size_t mtu_size = 20 - 1;  // 실제 데이터 크기
    size_t offset = 0;

    // 시작 패킷 전송 (3바이트: 헤더1 + 길이2)
    uint8_t startPacket[3] = {
        DATA_START_PACKET, 
        (uint8_t)((length >> 8) & 0xFF),  // 상위 바이트
        (uint8_t)(length & 0xFF)          // 하위 바이트
    };
    bleuart.write(startPacket, 3);
    delay(1);  // 안정성을 위한 지연

    // 데이터를 청크 단위로 분할하여 전송
    while (offset < length) {
        uint8_t chunkPacket[20];  // MTU 크기 + 헤더(1byte)
        chunkPacket[0] = DATA_CHUNK_PACKET;  // 청크 패킷 헤더
        
        size_t chunk_size = min(mtu_size, length - offset);
        memcpy(&chunkPacket[1], &data[offset], chunk_size);
        
        bleuart.write(chunkPacket, chunk_size + 1);
        offset += chunk_size;
        delay(1);  // 안정성을 위한 지연
        
        // DEBUG_PRINTF("Sent chunk: %d bytes, Offset: %d\n", chunk_size, offset);
    }

    // 종료 패킷 전송
    uint8_t endPacket[1] = {DATA_END_PACKET};
    bleuart.write(endPacket, 1);
    
    DEBUG_PRINTF("Large data transmission complete\n");
}

void BLEHandler::sendData(const uint8_t *data, size_t length)
{
    if (!isBLEConnected || length > MAX_BUFFER_SIZE) {
        DEBUG_PRINTF("Data too large for buffer\n");
        return;
    }

    if (!isBLEConnected)
    return;

    if (length > 20)
    {
        sendLargeData(data, length);
    }
    else
    {
        // BLE UART를 통해 데이터 전송
        bleuart.write(data, length);
    }

    // // 헤더 전송 (명령어 타입)
    // uint8_t header = cmdLargeData;
    // bleuart.write(&header, 1);
    // delay(10); // 안정성을 위한 지연

    // // 데이터를 청크 단위로 분할하여 전송
    // while (offset < length)
    // {
    //     size_t chunk_size = min(mtu_size, length - offset);
    //     bleuart.write(&data[offset], chunk_size);
    //     offset += chunk_size;
    //     delay(20); // 안정성을 위한 지연
    // }
}

void BLEHandler::processCommand()
{
    uint8_t buf[64];
    int count = bleuart.read(buf, sizeof(buf));

    if (count <= 0)
        return;

    switch (buf[0])
    {
    case cmdLED:
    case '1':
    {
        DEBUG_PRINTF("LED control\n");
        digitalWrite(LED_PIN, buf[1] ? HIGH : LOW);
        uint8_t response[2] = {cmdLED, buf[1]};
        sendData(response, 2);
        break;
    }
    case cmdStatus:
    case '2':
    {
        DEBUG_PRINTF("System status\n");
        uint8_t response[2] = {cmdStatus, 0x00};
        sendData(response, 2);
        break;
    }
    case cmdLargeData:
    case '3':
    {
        DEBUG_PRINTF("Large data transfer\n");
        uint8_t testBuffer[1024];
        uint16_t value = 0;
        for (uint16_t i = 0; i < 1024; i+=2)
        {   
            testBuffer[i] = (uint8_t)((value >> 8) & 0xFF);
            testBuffer[i + 1] = (uint8_t)(value & 0xFF);
            value++;
        }
        sendLargeData(testBuffer, 1024);
        break;
    }
    case cmdADC:
    case '4':
    {
        int adcValue = analogRead(ADC_PIN);
        uint8_t response[3] = {
            cmdADC,
            (uint8_t)((adcValue >> 8) & 0xFF),
            (uint8_t)(adcValue & 0xFF)};
        DEBUG_PRINTF("ADC Value: %d\n", adcValue);
        sendData(response, 3);
        break;
    }
    default:
    {
        DEBUG_PRINTF("Unknown command received\n");
        uint8_t response[2] = {cmdUnknown, 0x01};
        sendData(response, 2);
        break;
    }
    }
}

bool BLEHandler::getConnected() const
{
    return isBLEConnected;
}

void BLEHandler::setConnected(bool connected)
{
    isBLEConnected = connected;
}