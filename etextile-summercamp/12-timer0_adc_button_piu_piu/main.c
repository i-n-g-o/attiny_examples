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
#include <math.h>

// prescaler for timer0 - sets frequency range
#define TIMER0_DIV (_BV(CS01) | _BV(CS00)) // clk_io/16
#define TIMER0_DIV_MASK 7 // prescaler mask 0b00000111

// initial value for timer1
volatile uint8_t tcnt1_init = 55;

uint8_t counter = 0; // used to disort our frequency
volatile uint8_t counter_max = 0;



void pinSetup() {
	//--------------------------------
	// pin setup
	DDRB = ~(_BV(PB2) | _BV(PB3)); // PB2 (INT0) as input
	PORTB = _BV(PB2) | _BV(PB3); // turn on pullup
}

void setupInterrupt() {
	
	//--------------------------------
	// turn on interrupts
	GIMSK |= _BV(INT0); // Turn on External Interrupt Request 0
	
	//--------------------------------
	// set Interrupt Sense Control 0 Bit 1 and Bit 0 (for ISR0)
	// interrupt on falling edge
	MCUCR |= _BV(ISC01);
	MCUCR &= ~_BV(ISC00);
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
	TCCR0B &= ~TIMER0_DIV_MASK; // no clock source
	
	//--------------------------------
	// init OCR0A
	OCR0A = tcnt1_init;
	
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
	// preload timer value
	TCNT1 = tcnt1_init;
	
	//--------------------------------
	// disable output compare match
	TIMSK &= ~(_BV(OCIE1A) | _BV(OCIE1B));
	
	//--------------------------------
	// enable overflow interrupt
	TIMSK |= _BV(TOIE1);
}


// timer0 compare match A interrupt
ISR(TIM0_COMPA_vect) {
	
	// try out some transformations
	//	double val = (double)TCNT1/255.0;
	//	OCR0A = sin(val*10) * 255;
	
	if (++counter > counter_max) {
		counter = 0;
	}
	
	OCR0A = TCNT1 - counter;
}

// timer1 overflow interrupt
ISR(TIMER1_OVF_vect) {
	// turn off timer 0
	TCCR0B &= ~TIMER0_DIV_MASK;
}


// External Interrupt
ISR(INT0_vect) {
	// enable timer0
	TCCR0B |= TIMER0_DIV;
	// set value of TCNT1
	TCNT1 = tcnt1_init;
}


// main method
int main(void)
{
	pinSetup();
	setupADC();
	setupInterrupt();
	timer0Setup();
	timer1Setup();
	
	sei();
	
	for(;;) {
		_delay_ms(3);
		
		// read ADC
		uint16_t val = readADC();
		counter_max = (val / 16);
	}
	return 0;   /* never reached */
}
