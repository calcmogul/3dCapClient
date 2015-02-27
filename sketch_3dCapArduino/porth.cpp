#include "porth.h"
#include <avr/io.h>

uint8_t porth::port() {
    return PORTH;
}

void porth::port(uint8_t v) {
    PORTH = v;
}

uint8_t porth::pin() {
    return PINH;
}

void porth::pin(uint8_t v) {
    PINH = v;
}

uint8_t porth::dir() {
    return DDRH;
}

void porth::dir(uint8_t v) {
    DDRH = v;
}

