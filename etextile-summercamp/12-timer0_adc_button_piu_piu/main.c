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
#define TIMER0_DIV ((1 << CS01) | (1 << CS00)) // clk_io/16
#define TIMER0_DIV_MASK 7 // prescaler mask 0b00000111

// initial value for timer1
volatile uint8_t tcnt1_init = 55;

uint8_t counter = 0; // used to disort our frequency
volatile uint8_t counter_max = 0;



void pinSetup() {
	//--------------------------------
	// pin setup
	DDRB = ~((1 << PB2) | (1 << PB3)); // PB2 (INT0) as input
	PORTB = (1 << PB2) | (1 << PB3); // turn on pullup
}

void setupInterrupt() {
	
	//--------------------------------
	// turn on interrupts
	GIMSK |= (1 << INT0); // Turn on External Interrupt Request 0
	
	//--------------------------------
	// set Interrupt Sense Control 0 Bit 1 and Bit 0 (for ISR0)
	// interrupt on falling edge
	MCUCR |= (1 << ISC01);
	MCUCR &= ~(1 << ISC00);
}

void setupADC() {
	
	//--------------------------------
	// select ADC3 (PB3)
	ADMUX &= ~((1 << MUX3) | (1 << MUX2));
	ADMUX |= (1 << MUX1) | (1 << MUX0);
	
	//--------------------------------
	// VCC used as Voltage Reference
	ADMUX &= ~((1 << REFS0) | (1 << REFS2) | (1 << REFS1));
	
	//--------------------------------
	// ADLAR: ADC Left Adjust Result
	// select right aligned
	ADMUX &= ~((1 << ADLAR));
	
	//--------------------------------
	// ACME: Analog Comparator Multiplexer Enable
	// set ACME to 1 to select ADCx
	ADCSRB |= (1 << ACME);
	
	//--------------------------------
	// diable autotrigger
	ADCSRA &= ~(1 << ADATE);
	// enable ADC interrupt
	ADCSRA &= ~(1 << ADIE);
	
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
	TCCR0B &= ~(1 << WGM02);
	TCCR0A &= ~(1 << WGM00);
	TCCR0A |= (1 << WGM01);
	
	//--------------------------------
	// toggle pin OC0A on compare match
	TCCR0A &= ~(1 << COM0A1);
	TCCR0A |= (1 << COM0A0);
	
	//--------------------------------
	// set prescaler
	TCCR0B &= ~TIMER0_DIV_MASK; // no clock source
	
	//--------------------------------
	// init OCR0A
	OCR0A = tcnt1_init;
	
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
	// preload timer value
	TCNT1 = tcnt1_init;
	
	//--------------------------------
	// disable output compare match
	TIMSK &= ~((1 << OCIE1A) | (1 << OCIE1B));
	
	//--------------------------------
	// enable overflow interrupt
	TIMSK |= (1 << TOIE1);
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
