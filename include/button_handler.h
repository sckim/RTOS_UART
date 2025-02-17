#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <Arduino.h>
#include "config.h"

typedef enum {
    BUTTON_NO_EVENT,
    BUTTON_SHORT_PRESS,   
    BUTTON_MEDIUM_PRESS,  
    BUTTON_LONG_PRESS    
} ButtonEvent_t;

class ButtonHandler {
private:
    static volatile bool buttonPressed;
    static volatile unsigned long pressStartTime;
    const int pin;
    
    static const unsigned long SHORT_PRESS = SHORT_PRESS_TIME;  
    static const unsigned long MIDDLE_PRESS = LONG_PRESS_TIME;  
    
    static ButtonHandler* instance;
    static void buttonISR();

public:
    ButtonHandler(int buttonPin);

    void begin();
    static ButtonHandler* getInstance() { return instance; }
};

extern QueueHandle_t xButtonEventQueue;

#endif // BUTTON_HANDLER_H