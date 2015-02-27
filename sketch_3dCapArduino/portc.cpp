#include "portc.h"
#include <avr/io.h>

uint8_t portc::port() {
    return PORTC;
}

void portc::port(uint8_t v) {
    PORTC = v;
}

uint8_t portc::pin() {
    return PINC;
}

void portc::pin(uint8_t v) {
    PINC = v;
}

uint8_t portc::dir() {
    return DDRC;
}

void portc::dir(uint8_t v) {
    DDRC = v;
}

