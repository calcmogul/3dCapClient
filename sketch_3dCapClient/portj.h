// Copyright (c) Tyler Veness 2017. All Rights Reserved.

#ifndef PORTJ_H
#define PORTJ_H

#include <stdint.h>

struct portj {
    static uint8_t port();
    static void port(uint8_t v);

    static uint8_t pin();
    static void pin(uint8_t v);

    static uint8_t dir();
    static void dir(uint8_t v);
};

#endif  // PORTJ_H
