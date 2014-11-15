#include "porte.h"
#include <avr/io.h>

uint8_t porte::port() {
    return PORTE;
}

void porte::port( uint8_t v ) {
    PORTE = v;
}

uint8_t porte::pin() {
    return PINE;
}

void porte::pin( uint8_t v ) {
    PINE = v;
}

uint8_t porte::dir() {
    return DDRE;
}

void porte::dir( uint8_t v ) {
    DDRE = v;
}
