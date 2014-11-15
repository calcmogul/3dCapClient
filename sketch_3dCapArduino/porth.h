#ifndef PORTH_H
#define PORTH_H

#include <stdint.h>

struct porth {
    static uint8_t port();
    static void port( uint8_t v );

    static uint8_t pin();
    static void pin( uint8_t v );

    static uint8_t dir();
    static void dir( uint8_t v );
};

#endif // PORTH_H

