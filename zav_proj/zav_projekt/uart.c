
#include "uart.h"

//setup function
void uartInit(){
	UCSR0C = (1<<UCSZ00) | (1<<UCSZ01);		//8bit character, no parity, 1 stop bit
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);		//for enabling Rx and Tx pins
	UBRR0 = 25;			//baudrate 38400
}



