
// Blinking Without Using Delay on the ATtiny45

//                                     --------
//                                PB5 -|     |- VCC
//                                PB3 -|     |- PB2 
//                                PB4 -|     |- PB1                               
//                                GND -|     |- PB0 ------- to LED
//                                     --------


// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <avr/interrupt.h>

int speed = 120;

void pinSetup() {
	//--------------------------------
	// pin setup
	// -------- Inits --------- //
	DDRB = 0b00000001;            // Data Direction Register B: writing a one to the bit
	// enables output.
}

void timer0Setup() {
	//--------------------------------
	// CTC mode - counting direction up
	TCCR0B &= ~(1 << WGM02);
	TCCR0A &= ~(1 << WGM00);
	TCCR0A &= ~(1 << WGM01);
	
	
	//--------------------------------
	// set prescaler
	TCCR0B |= (1 << CS02) | (1 << CS00); // clk_io/1024
	
	//--------------------------------
	// preload timer counter
	TCNT0 = speed;
	
	//--------------------------------
	// enable overflow interrupt
	TIMSK |= (1 << TOIE0);
}

// timer0 overflow interrupt
ISR(TIMER0_OVF_vect) {

	// check if pin is high
	if( (PORTB & (1 << PB0)) == 0 ) {
		// pin is off - turn it on
		PORTB |= (1 << PB0);
	} else {
		// pin is on - turn it off
		PORTB &= ~(1 << PB0);
	}
	
	TCNT0 = speed;
}


int main(void) {

	pinSetup();
	
	timer0Setup();
	
	sei();
	
	// ------ Event loop ------ //
	while (1) {
		// idle
	}
	
	// This line is never reached
	return 0;
}
