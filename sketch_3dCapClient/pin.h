#ifndef PIN_H
#define PIN_H

#include "porta.h"
#include "portb.h"
#include "portc.h"
#include "portd.h"
#include "portg.h"
#include "portl.h"

template <typename Port, int Pin>
struct pin {
    static void set(bool value = true);
    static void clear();
    static void toggle();
    static bool get();
    static bool value();
    static void output(bool value);
    static bool output();

    static void make_output();
    static void make_input();
    static void make_low();
    static void make_high();
    static void set_value(bool value);
    static void set_high();
    static void set_low();
    static bool read();
    static void pullup();
};

template <typename Port, int Pin>
struct inv_pin {
    static void set(bool value = true);
    static void clear();
    static void toggle();
    static bool get();
    static bool value();
    static void output(bool value);
    static bool output();

    static void make_output();
    static void make_input();
    static void make_low();
    static void make_high();
    static void set_value(bool value);
    static void set_high();
    static void set_low();
    static bool read();
    static void pullup();
};

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

typedef pin<porta, 5> DIO27;
typedef pin<porta, 6> DIO28;
typedef pin<porta, 7> DIO29;
typedef pin<portc, 7> DIO30;
typedef pin<portc, 6> DIO31;
typedef pin<portc, 5> DIO32;
typedef pin<portc, 4> DIO33;
typedef pin<portc, 3> DIO34;
typedef pin<portc, 2> DIO35;
typedef pin<portc, 1> DIO36;
typedef pin<portc, 0> DIO37;
typedef pin<portd, 7> DIO38;
typedef pin<portg, 2> DIO39;
typedef pin<portg, 1> DIO40;
typedef pin<portg, 0> DIO41;
typedef pin<portl, 7> DIO42;
typedef pin<portl, 6> DIO43;
typedef pin<portl, 5> DIO44;
typedef pin<portl, 4> DIO45;
typedef pin<portl, 3> DIO46;
typedef pin<portl, 2> DIO47;
typedef pin<portl, 1> DIO48;
typedef pin<portl, 0> DIO49;
typedef pin<portb, 3> DIO50;
typedef pin<portb, 2> DIO51;
typedef pin<portb, 1> DIO52;
typedef pin<portb, 0> DIO53;

#endif // PIN_H

