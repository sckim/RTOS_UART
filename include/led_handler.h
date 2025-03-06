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
    void errorBlink();  // ���� ���¸� ��Ÿ���� ���� ����
};

#endif // LED_HANDLER_H