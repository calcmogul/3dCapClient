#include "portb.h"
#include <avr/io.h>

uint8_t portb::port() {
    return PORTB;
}

void portb::port(uint8_t v) {
    PORTB = v;
}

uint8_t portb::pin() {
    return PINB;
}

void portb::pin(uint8_t v) {
    PINB = v;
}

uint8_t portb::dir() {
    return DDRB;
}

void portb::dir(uint8_t v) {
    DDRB = v;
}

