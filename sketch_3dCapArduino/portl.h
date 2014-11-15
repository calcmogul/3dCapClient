#ifndef PORTL_H
#define PORTL_H

#include <stdint.h>

struct portl {
    static uint8_t port();
    static void port( uint8_t v );

    static uint8_t pin();
    static void pin( uint8_t v );

    static uint8_t dir();
    static void dir( uint8_t v );
};

#endif // PORTL_H

