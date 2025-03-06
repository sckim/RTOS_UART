#include "config.h"
#include "error_handler.h"

void errorHandler(const char* message) {
    DEBUG_PRINTF("%s\n", message);
    while (true) {
        digitalWrite(LED_RED, HIGH);
        delay(500);
        digitalWrite(LED_RED, LOW);
        delay(500);
    }
}