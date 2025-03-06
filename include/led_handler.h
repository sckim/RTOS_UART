#ifndef LED_HANDLER_H
#define LED_HANDLER_H

#include "config.h"

class LEDHandler {
private:
    const int pin;

public:
    LEDHandler(int ledPin);
    
    void begin();
    void turnOn();
    void turnOff();
    void shortBlink();
    void mediumBlink();
    void longBlink();
    void errorBlink();  // 에러 상태를 나타내는 무한 점멸
};

#endif // LED_HANDLER_H