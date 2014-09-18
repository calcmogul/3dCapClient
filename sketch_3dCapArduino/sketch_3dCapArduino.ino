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
    Serial.print( time( 22, 1 << 0 ), DEC );
    Serial.print( " " );
    Serial.print( time( 24, 1 << 2 ), DEC );
    Serial.print( " " );
    Serial.println( time( 26, 1 << 4 ), DEC );
}

long time( int pin, byte mask ) {
  unsigned long count = 0, total = 0;
  while( checkTimer() < refresh ) {
      // pinMode() is about 6 times slower than assigning DDRB directly, but
      // that pause is important. (probably for preventing an overflow in
      // 'total'). delay() doesn't have a high enough granularity at 1ms since
      // the pinMode() calls only take about 6μs each.
      pinMode( pin, OUTPUT );
      PORTA = 0;
      pinMode( pin, INPUT );

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
    return ((timer0_overflow_count << 8) + TCNT0) << 2;
}

