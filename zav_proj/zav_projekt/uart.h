
#ifndef UART_H_
#define UART_H_


#include <stdio.h>
#include <avr/io.h>

void uartInit();
static int uart_putchar(char c, FILE *stream);		//for character printing
static FILE uart = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);	//file setup for print target

static int uart_putchar(char c, FILE *stream){
	if (c == '\n')
	uart_putchar('\r', stream);
	while(!(UCSR0A & (1<<UDRE0))){}		//until data register empty
	UDR0 = c;							//write to data register
	return 0;
}
#endif /* UART_H_ */