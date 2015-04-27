//
// By Kyle McDonald
// Modified by Tyler Veness
// From the instructables project at:
// http://www.instructables.com/id/DIY-3D-Controller/

#include "pin.h"

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

#define resolution 8
#define mains 60 // 60: north america, japan; 50: most other places

#define refresh 2 * 1000000 / mains

void setup() {
    Serial.begin(115200);

    // unused pins are fairly insignificant,
    // but pulled low to reduce unknown variables
    for (int i = 22; i <= 53; i++) {
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
    }

    for (int i = 22; i <= 26; i += 2) {
        pinMode(i, INPUT);
    }

    restartTimer();
}

long x;
long y;
long z;

void loop() {
    x = chargeTime(22);
    y = chargeTime(24);
    z = chargeTime(26);

/*
 *   // 27-35 x-axis
 *   DIO27::set( LOW );
 *   DIO28::set( LOW );
 *   DIO29::set( LOW );
 *   DIO30::set( LOW );
 *   DIO31::set( LOW );
 *   DIO32::set( LOW );
 *   DIO33::set( LOW );
 *   DIO34::set( LOW );
 *   DIO35::set( LOW );
 *
 *   // 36-44 y-axis
 *   DIO36::set( LOW );
 *   DIO37::set( LOW );
 *   DIO38::set( LOW );
 *   DIO39::set( LOW );
 *   DIO40::set( LOW );
 *   DIO41::set( LOW );
 *   DIO42::set( LOW );
 *   DIO43::set( LOW );
 *   DIO44::set( LOW );
 *
 *   // 45-53 z-axis
 *   DIO45::set( LOW );
 *   DIO46::set( LOW );
 *   DIO47::set( LOW );
 *   DIO48::set( LOW );
 *   DIO49::set( LOW );
 *   DIO50::set( LOW );
 *   DIO51::set( LOW );
 *   DIO52::set( LOW );
 *   DIO53::set( LOW );*/

    Serial.print(x, DEC);
    Serial.print(" ");
    Serial.print(y, DEC);
    Serial.print(" ");
    Serial.println(z, DEC);
}

/* Returns time taken for the pin to accumulate charge after set to 0V.
 * Charge is finished accumulating when the pin's state flips from 0 to 1.
 */
long chargeTime(int pin) {
    unsigned long count = 0, total = 0;

    /* On PORTA, pins 22 through 29 belong to a mask which starts with a left
     * bitshift of 0.
     */
    byte mask = 1 << (pin - 22);

    // Repeat resetting pin and counting until enough samples are accumulated
    while (checkTimer() < refresh) {
        /* Assigning DDRB is too fast for the granularity we want in the return
         * value. NOP instructions are added after each assignment to slow
         * the code down to the speed at which pinMode() operates so larger
         * values can be accumulated in the counter.
         */

        // Set pin to low
        PORTA &= ~mask;

        // Set pin to OUTPUT
        DDRA |= mask;

        asm volatile (
            // 10
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"

            // 20
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"

            // 30
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"

            // 40
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"

            // 50
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"

            // 60
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"

            // 70
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"

            // 80
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"

            // 90
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"

            // 100
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"

            // 110
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            );

        // Set pin to INPUT
        DDRA &= ~mask;

        // Count until pin switches state (until capacitor charges)
        while ((PINA & mask) == 0) {
            count++;
        }
        total++;
    }

    restartTimer();
    return (count << resolution) / total;
}

extern volatile unsigned long timer0_overflow_count;

void restartTimer() {
    timer0_overflow_count = 0;
    TCNT0 = 0;
}

unsigned long checkTimer() {
    /* Left bitshift of 2 was " * (64 / clockCyclesPerMicrosecond())"
     * = 64 / 16
     * = 4
     * Multiplying by 4 is same as left bitshift of 2
     */
    return ((timer0_overflow_count << 8) | TCNT0) << 2;
}

