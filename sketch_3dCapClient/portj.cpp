// Copyright (c) Tyler Veness 2017. All Rights Reserved.

#include "portj.h"

#include <avr/io.h>

uint8_t portj::port() { return PORTJ; }

void portj::port(uint8_t v) { PORTJ = v; }

uint8_t portj::pin() { return PINJ; }

void portj::pin(uint8_t v) { PINJ = v; }

uint8_t portj::dir() { return DDRJ; }

void portj::dir(uint8_t v) { DDRJ = v; }
