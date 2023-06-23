#include "buttons.h"
#include <avr/io.h>

//variable for tracking which button is pressed
uint8_t buttonsStatus = 0;

//setup function for buttons
void buttonsInit() {
	ADMUX = (1<<REFS0);	// select voltage reference and mux input
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);	// set prescaler division factor to 128 and enable ADC
}

//reads the input to determine which button is pressed
void buttonsRead() {
	ADMUX &= ~0x0F;			// select channel 0
	ADCSRA |= (1<<ADSC);	// start ADC conversion
	while (ADCSRA&(1<<ADSC)) {}	// wait for conversion to complete
	
	//no button is pressed
	if ((buttonsStatus!= 0)&&(ADC > 1000)){		
		buttonsStatus = 0;
	}
	//some button is pressed
	else if (buttonsStatus == 0){	//
		if((70 < ADC) && (ADC < 120)) buttonsStatus = 3;			//up
		else if((230 < ADC) && (ADC < 280)) buttonsStatus = 4;		//down
		else if((610 < ADC) && (ADC < 660)) buttonsStatus = 1;		//select
		else if((380 < ADC) && (ADC < 430)) buttonsStatus = 2;		//left
		else if(20 > ADC) buttonsStatus = 5;						//right
	}
}

//outputs the information about button press 
uint8_t buttonsGet(){
	return buttonsStatus;
}