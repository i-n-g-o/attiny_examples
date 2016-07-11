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
#include <util/delay.h>


void pinSetup() {
	//--------------------------------
	// pin setup
	DDRB = ~_BV(PB3); // PB2 (INT0) as input
	PORTB = _BV(PB3); // turn on pullup
}

void setupADC() {
	
	//--------------------------------
	// select ADC3 (PB3)
	ADMUX &= ~(_BV(MUX3) | _BV(MUX2));
	ADMUX |= _BV(MUX1) | _BV(MUX0);
	
	//--------------------------------
	// VCC used as Voltage Reference
	ADMUX &= ~(_BV(REFS0) | _BV(REFS2) | _BV(REFS1));
	
	//--------------------------------
	// ADLAR: ADC Left Adjust Result
	// select right aligned
	ADMUX &= ~(_BV(ADLAR));
	
	//--------------------------------
	// ACME: Analog Comparator Multiplexer Enable
	// set ACME to 1 to select ADCx
	ADCSRB |= _BV(ACME);
	
	//--------------------------------
	// diable autotrigger
	ADCSRA &= ~_BV(ADATE);
	// enable ADC interrupt
	ADCSRA &= ~_BV(ADIE);
	
	//--------------------------------
	// ADC clock prescaler /8
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0);
	
	//--------------------------------
	// enable ADC
	ADCSRA |= (1 << ADEN);
}

uint16_t readADC() {
	// trigger sample
	ADCSRA |= (1 << ADSC);
	// wait until its done
	loop_until_bit_is_clear(ADCSRA, ADSC);
	// return value
	return (ADC);
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
	// init OCR0A - set frequency
	OCR0A = 55;
	
	//--------------------------------
	// enable output compare match
	TIMSK |= _BV(OCIE0A);
}


// timer0 compare match A interrupt
ISR(TIM0_COMPA_vect) {
	uint16_t val = readADC();
	OCR0A = 20 + (val / 16);
}


// main method
int main(void)
{
	pinSetup();
	setupADC();
	timer0Setup();
	
	sei();
	
	for(;;) {
		// idle
	}
	return 0;   /* never reached */
}
