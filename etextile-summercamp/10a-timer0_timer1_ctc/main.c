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

// prescaler for timer0 - sets frequency range
#define TIMER0_DIV (_BV(CS01) | _BV(CS00)) // clk_io/16
#define TIMER0_DIV_MASK 7 // prescaler mask 0b00000111

// initial value for timer1
volatile uint8_t tcnt1_init = 55;




void pinSetup() {
	//--------------------------------
	// pin setup
	DDRB = 0xFF; // all output
	PORTB = 0;
}


void timer0Setup() {
	//--------------------------------
	// CTC mode - counting direction up
	TCCR0B &= ~_BV(WGM02);
	TCCR0A &= ~_BV(WGM00);
	TCCR0A |= _BV(WGM01);
	
	//--------------------------------
	// toggle pin OC0A on compare match
	TCCR0A &= ~_BV(COM0A1);
	TCCR0A |= _BV(COM0A0);
	
	//--------------------------------
	// set prescaler
	TCCR0B |= _BV(CS01) | _BV(CS00); // clk_io/16
	
	//--------------------------------
	// init OCR0A
	OCR0A = 55;
	
	//--------------------------------
	// enable output compare match
	TIMSK |= _BV(OCIE0A);
}


void timer1Setup() {
	//--------------------------------
	// don't cleare on compare match
	TCCR1 &= ~_BV(CTC1);
	// disable PWM
	TCCR1 &= ~_BV(PWM1A);
	
	//--------------------------------
	// set prescaler - make it slow (PCK/16384)
	TCCR1 |= _BV(CS13) | _BV(CS11) | _BV(CS12) | _BV(CS10);
	
	//--------------------------------
	// toggle OC1A: PB1
	TCCR1 &= ~_BV(COM1A1);
	TCCR1 |= _BV(COM1A0);
	
	//--------------------------------
	// disable output compare match
	TIMSK &= ~(_BV(OCIE1A) | _BV(OCIE1B));
	
	//--------------------------------
	// enable overflow interrupt
	TIMSK |= _BV(TOIE1);
}


// timer0 compare match A interrupt
ISR(TIM0_COMPA_vect) {
	OCR0A = TCNT1;
}


// main method
int main(void)
{
	pinSetup();
	timer0Setup();
	timer1Setup();
	
	sei();
	
	for(;;) {
		// idle
	}
	return 0;   /* never reached */
}
