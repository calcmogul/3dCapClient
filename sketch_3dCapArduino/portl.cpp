#include "portl.h"
#include <avr/io.h>

uint8_t portl::port() {
    return PORTL;
}

void portl::port( uint8_t v ) {
    PORTL = v;
}

uint8_t portl::pin() {
    return PINL;
}

void portl::pin( uint8_t v ) {
    PINL = v;
}

uint8_t portl::dir() {
    return DDRL;
}

void portl::dir( uint8_t v ) {
    DDRL = v;
}
