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

#define PIN_4 4
#define PIN_3 3

uint8_t switches = 0;
uint8_t switchesMask = 0b00000111;

void setupInterrupt() {
	
	//--------------------------------
	// pin setup
	DDRB = ~switchesMask; // PB0 (PCINT0), PB1 (PCINT1), PB2 (PCINT2) as input
	PORTB |= switchesMask; // turn on pullup
	
	// intial read state of switches
	switches = PINB & switchesMask;
	
	//--------------------------------
	// turn on interrupts
	GIMSK |= _BV(PCIE); // Turn on Pin Change Interrupt
	GIMSK |= _BV(INT0); // Turn on External Interrupt Request 0
	
	//--------------------------------
	// register pins for PCINT interrupt
	PCMSK |= _BV(PCINT0); // PB0
	PCMSK |= _BV(PCINT1); // PB1
	
	//--------------------------------
	// set Interrupt Sense Control 0 Bit 1 and Bit 0 (for ISR0)
	// interrupt on falling edge
	MCUCR |= _BV(ISC01);
	MCUCR &= ~_BV(ISC00);
	
	//--------------------------------
	// turn on global interrupt bit
	SREG |= (1 << 7);
//	SREG &= ~(1 << 7); // turn off global interrupt
}


// Interrupt Service Routing
// Pin Change Interrupt
ISR(PCINT0_vect) {
	
	// check out which switch changed
	uint8_t _switches = (PINB & switchesMask);
	uint8_t changed = switches ^ _switches;
	
	if (changed & (1 << 0)) {
		//PB0 changed
		
		// turn PB4 off
		PORTB &= ~(1 << PIN_4);
	}
	if (changed & (1 << 1)) {
		//PB1 changed
		
		// turn PB4 on
		PORTB |= (1 << PIN_4);
	}
	if (changed & (1 << 2)) {
		//PB2 changed
		
		// change PB4
		if (PORTB & (1 << PIN_4)) {
			PORTB &= ~(1 << PIN_4);
		} else {
			PORTB |= (1 << PIN_4);
		}
	}
	
	// store switches state
	switches = _switches;
}

// Interrupt Service Routing
// External Interrupt
ISR(INT0_vect) {
	
	if (PORTB & (1 << PIN_3)) {
		PORTB &= ~(1 << PIN_3);
	} else {
		PORTB |= (1 << PIN_3);
	}
	
}



// main method
int main(void)
{
	setupInterrupt();
    for(;;){
        // idle. all is done in interrupts
    }
    return 0;   /* never reached */
}
