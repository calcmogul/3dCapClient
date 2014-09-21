//
// By Kyle McDonald
// Modified by Tyler Veness
// From the instructables project at:
// http://www.instructables.com/id/DIY-3D-Controller/

#define resolution 8
#define mains 60 // 60: north america, japan; 50: most other places

#define refresh 2 * 1000000 / mains

void setup() {
    Serial.begin( 115200 );

    // unused pins are fairly insignificant,
    // but pulled low to reduce unknown variables
    for ( int i = 22 ; i <= 53 ; i++ ) {
        pinMode( i, OUTPUT );
        digitalWrite( i, LOW );
    }

    for ( int i = 22 ; i <= 26 ; i += 2 ) {
        pinMode( i, INPUT );
    }

    restartTimer();
}

void loop() {
    Serial.print( chargeTime( 22 ), DEC );
    Serial.print( " " );
    Serial.print( chargeTime( 24 ), DEC );
    Serial.print( " " );
    Serial.println( chargeTime( 26 ), DEC );
}

/* Returns time taken for the pin to accumulate charge after set to 0V.
 * Charge is finished accumulating when the pin's state flips from 0 to 1.
 */
long chargeTime( int pin ) {
    unsigned long count = 0, total = 0;

    /* On PORTA, pins 22 through 29 belong to a mask which starts with a left
     * bitshift of 0.
     */
    byte mask = 1 << (pin - 22);

    // Repeat resetting pin and counting until enough samples are accumulated
    while( checkTimer() < refresh ) {
        /* Assigning DDRB is too fast for the granularity we want in the return
         * value. 60 NOP instructions are added after each assignment to slow
         * the code down to the speed at which pinMode() operates so larger
         * values can be accumulated in the counter.
         */

        // Set pin to low
        PORTA &= ~mask;

        // Set pin to OUTPUT
        DDRA |= mask;

        asm volatile(
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

            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
        );

        // Set pin to INPUT
        DDRA &= ~mask;

        asm volatile(
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

            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
        );

        // Count until pin switches state (until capacitor charges)
        while ( (PINA & mask) == 0 ) {
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

