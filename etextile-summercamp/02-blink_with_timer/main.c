
// Blinking With Delay on the ATtiny45

//                                     --------
//        (PCINT5/!RESET/ADC0/dW) PB5 -|     |- VCC
// (PCINT3/XTAL1/CLKI/!OC1B/ADC3) PB3 -|     |- PB2 (SCK/USCK/SCL/ADC1/T0/INT0/PCINT2)
//  (PCINT4/XTAL2/CLKO/OC1B/ADC2) PB4 -|     |- PB1 (MISO/DO/AIN1/OC0B/OC1A/PCINT1)                               
//                                GND -|     |- PB0 (MOSI/DI/SDA/AIN0/OC0A/!OC1A/AREF/PCINT0)
//                                     --------


// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <util/delay.h>                     /* Functions to waste time */


int main(void) {

  // -------- Inits --------- //
  DDRB = 0b0000001;            // Data Direction Register B: writing a one to the bit
                               // enables output.

  // ------ Event loop ------ //
  while (1) {

    PORTB = 0b00000001;          // Turn on first LED bit/pin in PORTB 
    _delay_ms(1000);             // wait

    PORTB = 0b00000000;          // Turn off all B pins, including LED
    _delay_ms(1000);             // wait 

  }                              // End event loop 
  return 0;                      // This line is never reached
}
