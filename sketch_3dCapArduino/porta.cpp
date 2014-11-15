#include "porta.h"
#include <avr/io.h>

uint8_t porta::port() {
    return PORTA;
}

void porta::port( uint8_t v ) {
    PORTA = v;
}

uint8_t porta::pin() {
    return PINA;
}

void porta::pin( uint8_t v ) {
    PINA = v;
}

uint8_t porta::dir() {
    return DDRA;
}

void porta::dir( uint8_t v ) {
    DDRA = v;
}
