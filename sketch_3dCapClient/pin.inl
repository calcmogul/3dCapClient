#include "pin.h"

template <typename Port, int Pin>
void pin<Port, Pin>::set(bool value) {
    if (value) {
        Port::port(Port::port() | (1 << Pin));
    }
    else {
        Port::port(Port::port() & ~(1 << Pin));
    }
}

template <typename Port, int Pin>
void pin<Port, Pin>::clear() {
    set(false);
}

template <typename Port, int Pin>
void pin<Port, Pin>::toggle() {
    Port::port(Port::port() ^ (1 << Pin));
}

template <typename Port, int Pin>
bool pin<Port, Pin>::get() {
    return (Port::port() & (1 << Pin)) != 0;
}

template <typename Port, int Pin>
bool pin<Port, Pin>::value() {
    return (Port::pin() & (1 << Pin)) != 0;
}

template <typename Port, int Pin>
void pin<Port, Pin>::output(bool value) {
    if (value) {
        Port::dir(Port::dir() | (1 << Pin));
    }
    else {
        Port::dir(Port::dir() & ~(1 << Pin));
    }
}

template <typename Port, int Pin>
bool pin<Port, Pin>::output() {
    return (Port::dir() & (1 << Pin)) != 0;
}

template <typename Port, int Pin>
void pin<Port, Pin>::make_output() {
    output(true);
}

template <typename Port, int Pin>
void pin<Port, Pin>::make_input() {
    output(false);
    clear();
}

template <typename Port, int Pin>
void pin<Port, Pin>::make_low() {
    clear();
    output(true);
}

template <typename Port, int Pin>
void pin<Port, Pin>::make_high() {
    set();
    output(true);
}

template <typename Port, int Pin>
void pin<Port, Pin>::set_value(bool value) {
    set(value);
}

template <typename Port, int Pin>
void pin<Port, Pin>::set_high() {
    set(true);
}

template <typename Port, int Pin>
void pin<Port, Pin>::set_low() {
    set(false);
}

template <typename Port, int Pin>
bool pin<Port, Pin>::read() {
    return value();
}

template <typename Port, int Pin>
void pin<Port, Pin>::pullup() {
    set_high();
}

template <typename Port, int Pin>
void inv_pin<Port, Pin>::set(bool value) {
    if (value) {
        Port::port(Port::port() & ~(1 << Pin));
    }
    else {
        Port::port(Port::port() | (1 << Pin));
    }
}

template <typename Port, int Pin>
void inv_pin<Port, Pin>::clear() {
    set(false);
}

template <typename Port, int Pin>
void inv_pin<Port, Pin>::toggle() {
    Port::port(Port::port() ^ (1 << Pin));
}

template <typename Port, int Pin>
bool inv_pin<Port, Pin>::get() {
    return (Port::port() & (1 << Pin)) == 0;
}

template <typename Port, int Pin>
bool inv_pin<Port, Pin>::value() {
    return (Port::pin() & (1 << Pin)) == 0;
}

template <typename Port, int Pin>
void inv_pin<Port, Pin>::output(bool value) {
    if (value) {
        Port::dir(Port::dir() | (1 << Pin));
    }
    else {
        Port::dir(Port::dir() & ~(1 << Pin));
    }
}

template <typename Port, int Pin>
bool inv_pin<Port, Pin>::output() {
    return (Port::dir() & (1 << Pin)) != 0;
}

template <typename Port, int Pin>
void inv_pin<Port, Pin>::make_output() {
    output(true);
}

template <typename Port, int Pin>
void inv_pin<Port, Pin>::make_input() {
    output(false);
    clear();
}

template <typename Port, int Pin>
void inv_pin<Port, Pin>::make_low() {
    clear();
    output(true);
}

template <typename Port, int Pin>
void inv_pin<Port, Pin>::make_high() {
    set();
    output(true);
}

template <typename Port, int Pin>
void inv_pin<Port, Pin>::set_value(bool value) {
    set(value);
}

template <typename Port, int Pin>
void inv_pin<Port, Pin>::set_high() {
    set(true);
}

template <typename Port, int Pin>
void inv_pin<Port, Pin>::set_low() {
    set(false);
}

template <typename Port, int Pin>
bool inv_pin<Port, Pin>::read() {
    return value();
}

template <typename Port, int Pin>
void inv_pin<Port, Pin>::pullup() {
    set_low();
}

