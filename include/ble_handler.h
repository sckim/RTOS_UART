#ifndef BLE_HANDLER_H
#define BLE_HANDLER_H

#include <bluefruit.h>
#include "config.h"

#define DATA_START_PACKET 0x10
#define DATA_CHUNK_PACKET 0x11
#define DATA_END_PACKET 0x12

extern QueueHandle_t xBLEEventQueue;

// BLE 이벤트 타입 정의
typedef enum {
    BLE_EVENT_CONNECTED,
    BLE_EVENT_DISCONNECTED,
    BLE_EVENT_DATA_SEND=3,
    BLE_EVENT_ADC_DONE=5 
} BLEEventType_t;

// BLE 이벤트 구조체
typedef struct {
    BLEEventType_t eventType;
    uint8_t data[20];
    uint16_t length;
} BLEEvent_t;

class BLEHandler {   
private:
    static BLEHandler* instance;
    BLEUart bleuart;
    bool isBLEConnected;
    
    // 콜백 함수들을 static으로 선언
    static void connect_callback(uint16_t conn_handle);
    static void disconnect_callback(uint16_t conn_handle, uint8_t reason);
    static void bleuart_rx_callback(uint16_t conn_handle);

public:
    BLEHandler();
    void init();
    void startAdv(void);
    void sendLargeData(const uint8_t* data, size_t length);
    void sendData(const uint8_t* data, size_t length);
    bool getConnected() const;
    void setConnected(bool connected);
    void processCommand();
};

#endif // BLE_HANDLER_H