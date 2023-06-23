
#include <stdbool.h>
#include <avr/io.h>
#include "relays.h"

//variable for keeping relay states
bool relayState[2] = {1,0};		//initialized by default values (locked, no alarm)

//setup function
void relayInit(){
	DDRC |= (1<<DDC2) | (1<<DDC1);		//sets control pins to output
	relayControl(1,relayState[0]);		//sets relay1
	relayControl(2,relayState[1]);		//sets relay2
}

//a function for manipulating the relays
void relayControl(uint8_t relayNumber, bool action){
	switch (relayNumber)
	{
		//lock
		case 1:
			//depending on action, set relay value
			if (!action) PORTC |= (1<<PORTC1);
			else PORTC &= ~(1<<PORTC1);
			break;
		//alarm
		case 2:
			//depending on action, set relay value
			if (!action) PORTC |= (1<<PORTC2);
			else PORTC &= ~(1<<PORTC2);
			break;
		default:
			break;
	}
	//update the variable value
	relayState[relayNumber-1] = action;
}

//for outputting the relays value
bool relayStatus(uint8_t relayNumber){
	return relayState[relayNumber-1];
}
