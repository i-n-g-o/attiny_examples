
// Button Read on the ATtiny45

//                                     --------
//        (PCINT5/!RESET/ADC0/dW) PB5 -|     |- VCC
// (PCINT3/XTAL1/CLKI/!OC1B/ADC3) PB3 -|     |- PB2 (SCK/USCK/SCL/ADC1/T0/INT0/PCINT2)
//  (PCINT4/XTAL2/CLKO/OC1B/ADC2) PB4 -|     |- PB1 (MISO/DO/AIN1/OC0B/OC1A/PCINT1)   ------button to ground                     
//                                GND -|     |- PB0 (MOSI/DI/SDA/AIN0/OC0A/!OC1A/AREF/PCINT0) ----- to LED
//                                     --------


// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <util/delay.h>                     /* Functions to waste time */


int main(void) {

  // -------- Inits --------- //
  DDRB = 0b0000001;            // Data Direction Register B: writing a one to the bit
                               // enables output, 0 an input.
  PORTB |= (1 << PB1);         // turns on internal pullup resistor

  // ------ Event loop ------ //
  while (1) {

    if( PINB & (1<<PB1) == 0) {           // check if pin is high

      PORTB |= (1 << PB0);           // Turn on first LED bit/pin in PORTB 
      _delay_ms(1000);               // wait

      PORTB ^= (1 << PB0);           // Turn off LED B pin, including LED
      _delay_ms(1000);               // wait 
    } 
  }                              // End event loop 
  return 0;                      // This line is never reached
}
