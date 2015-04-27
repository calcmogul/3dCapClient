#include "portd.h"
#include <avr/io.h>

uint8_t portd::port() {
    return PORTD;
}

void portd::port(uint8_t v) {
    PORTD = v;
}

uint8_t portd::pin() {
    return PIND;
}

void portd::pin(uint8_t v) {
    PIND = v;
}

uint8_t portd::dir() {
    return DDRD;
}

void portd::dir(uint8_t v) {
    DDRD = v;
}

