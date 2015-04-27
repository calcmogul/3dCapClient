#include "portf.h"
#include <avr/io.h>

uint8_t portf::port() {
    return PORTF;
}

void portf::port(uint8_t v) {
    PORTF = v;
}

uint8_t portf::pin() {
    return PINF;
}

void portf::pin(uint8_t v) {
    PINF = v;
}

uint8_t portf::dir() {
    return DDRF;
}

void portf::dir(uint8_t v) {
    DDRF = v;
}

