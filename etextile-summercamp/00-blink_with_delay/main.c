
// Blinking With Delay on the ATtiny45

//                                     --------
//        						  PB5 -|     |- VCC
// 								  PB3 -|     |- PB2 
//  							  PB4 -|     |- PB1                               
//                                GND -|     |- PB0 ------- to LED
//                                     --------


// ------- Preamble -------- //
#include <avr/io.h>        // Defines pins, ports, etc
#include <util/delay.h>    // Functions to waste time 


int main(void) {

  // -------- Inits --------- //
  // Data Direction Register B: 
  //writing a one to the bit enables output.
  DDRB = 0b0000001;     

  // ------ Event loop ------ //
  while (1) {
  	// Turn on first LED bit/pin in PORTB 
    PORTB = 0b00000001; 
    // wait         
    _delay_ms(1000);             

    // Turn off all B pins, including LED
    PORTB = 0b00000000; 
    // wait         
    _delay_ms(1000);              

  }  
  // This line is never reached                           
  return 0;                      
}
