// Copyright (c) Tyler Veness 2017. All Rights Reserved.

#include "portk.h"

#include <avr/io.h>

uint8_t portk::port() { return PORTK; }

void portk::port(uint8_t v) { PORTK = v; }

uint8_t portk::pin() { return PINK; }

void portk::pin(uint8_t v) { PINK = v; }

uint8_t portk::dir() { return DDRK; }

void portk::dir(uint8_t v) { DDRK = v; }
