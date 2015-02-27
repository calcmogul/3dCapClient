#ifndef PORTK_H
#define PORTK_H

#include <stdint.h>

struct portk {
    static uint8_t port();
    static void port(uint8_t v);

    static uint8_t pin();
    static void pin(uint8_t v);

    static uint8_t dir();
    static void dir(uint8_t v);
};

#endif // PORTK_H

