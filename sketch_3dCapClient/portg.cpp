// Copyright (c) Tyler Veness 2017. All Rights Reserved.

#include "portg.h"

#include <avr/io.h>

uint8_t portg::port() { return PORTG; }

void portg::port(uint8_t v) { PORTG = v; }

uint8_t portg::pin() { return PING; }

void portg::pin(uint8_t v) { PING = v; }

uint8_t portg::dir() { return DDRG; }

void portg::dir(uint8_t v) { DDRG = v; }
