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
#define TIMER0_DIV ((1 << CS01) | (1 << CS00)) // clk_io/16
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
	// init OCR0A
	OCR0A = 55;
	
	//--------------------------------
	// enable output compare match
	TIMSK |= (1 << OCIE0A);
}


void timer1Setup() {
	//--------------------------------
	// don't cleare on compare match
	TCCR1 &= ~(1 << CTC1);
	// disable PWM
	TCCR1 &= ~(1 << PWM1A);
	
	//--------------------------------
	// set prescaler - make it slow (PCK/16384)
	TCCR1 |= (1 << CS13) | (1 << CS11) | (1 << CS12) | (1 << CS10);
	
	//--------------------------------
	// toggle OC1A: PB1
	TCCR1 &= ~(1 << COM1A1);
	TCCR1 |= (1 << COM1A0);
	
	//--------------------------------
	// disable output compare match
	TIMSK &= ~((1 << OCIE1A) | (1 << OCIE1B));
	
	//--------------------------------
	// enable overflow interrupt
	TIMSK |= (1 << TOIE1);
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
