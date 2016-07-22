/*
 * Pinout ATtiny25/45/85
 *
 *                                        --------
 *          (PCINT5/!RESET/ADC0/dW) PB5 -|        |- VCC
 *   (PCINT3/XTAL1/CLKI/!OC1B/ADC3) PB3 -|        |- PB2 (SCK/USCK/SCL/ADC1/T0/INT0/PCINT2)
 *    (PCINT4/XTAL2/CLKO/OC1B/ADC2) PB4 -|        |- PB1 (MISO/DO/AIN1/OC0B/OC1A/PCINT1)
 *                                  GND -|        |- PB0 (MOSI/DI/SDA/AIN0/OC0A/!OC1A/AREF/PCINT0)
 *                                        --------
 *
 */

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>


void pinSetup() {
	//--------------------------------
	// pin setup
	DDRB = 0xFF; // all output
	PORTB = 0;
}

void timer0Setup() {
	//--------------------------------
	// CTC mode - counting direction up
	TCCR0B &= ~(1 << WGM02);
	TCCR0A &= ~(1 << WGM00);
	TCCR0A |= (1 << WGM01);
	
	//--------------------------------
	// toggle pin OC0A on compare match
	TCCR0A &= ~(1 << COM0A1);
	TCCR0A |= (1 << COM0A0);
	
	//--------------------------------
	// set prescaler
	TCCR0B |= (1 << CS01) | (1 << CS00); // clk_io/16
	
	//--------------------------------
	// init OCR0A - set frequency
	OCR0A = 55;
	
	//--------------------------------
	// enable output compare match
	TIMSK |= (1 << OCIE0A);
}


// main method
int main(void)
{
	pinSetup();
	timer0Setup();
	
	sei();
	
	for(;;) {
		// idle
	}
	return 0;   /* never reached */
}
