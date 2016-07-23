
// Button Read on the ATtiny45

//                                     --------
//                                PB5 -|     |- VCC
//                                PB3 -|     |- PB2 
//                                PB4 -|     |- PB1 -------- to button to ground                    
//                                GND -|     |- PB0 ------- to LED
//                                     --------                              


// ------- Preamble -------- //
#include <avr/io.h>      // Defines pins, ports, etc
#include <util/delay.h>  // Functions to waste time


int main(void) {

  // -------- Inits --------- //
  // Data Direction Register B: writing a one to the bit
  // enables output, 0 an input.
  DDRB = 0b0000001;  
  // turn on internal pullup resistor          
  PORTB |= (1 << PB1);         

  // ------ Event loop ------ //
  while (1) {
    // check if pin is high
    if( (PINB & (1<<PB1)) == 0) {           
      // Turn on first LED bit/pin in PORTB 
      PORTB |= (1 << PB0); 
      // wait          
      _delay_ms(1000);               

      // Turn off LED B pin, including LED
      PORTB ^= (1 << PB0); 
      // wait           
      _delay_ms(1000);               
    } 
  }     
  // This line is never reached 
  return 0;                      
}
