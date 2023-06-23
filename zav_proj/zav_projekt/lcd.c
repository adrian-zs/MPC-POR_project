

#include <stdbool.h>
#include <stdio.h>
#include <avr/io.h>
#include <stdint-gcc.h>
#include "lcd.h"
#define F_CPU 16000000UL
#include <util/delay.h>

//control pins defined as constants
#define RS  PORTB0
#define E PORTB1
#define D4 PORTD4
#define D5 PORTD5
#define D6 PORTD6
#define D7 PORTD7

//short pulse on enable pin
void enaPulse(){
	PORTB |= (1<<E);
	_delay_us(1);
	PORTB &= ~(1<<E);
}

//function for sending instructions to the LCD
void lcdSendInstr(bool rs, uint8_t instr){
	//RS selects instruction or data register
	if(rs) PORTB |= (1<<RS);
	else PORTB &= ~(1<<RS);
	
	PORTD = (PORTD & 0x0F) | (instr & 0xF0);			//upper nibble
	enaPulse();
	_delay_us(10);
	PORTD = (PORTD & 0x0F) | ((instr << 4) & 0xF0);		//lower nibble
	enaPulse();
	_delay_us(50);
}

//setup for LCD 
void lcdInit(){
	//setting control pins as output
	DDRB = (1<<RS) | (1<<E);					
	DDRD = (1<<D4) | (1<<D5) | (1<<D6) | (1<<D7);
	
	lcdSendInstr(0,0x28); //function set
	lcdSendInstr(0,0x0C); //display on
	lcdClear(); //display clear
	lcdHome();	//cursor to pos 0
}

//sends the clear instruction to LCD
void lcdClear(){
	lcdSendInstr(0,0x01);
	_delay_us(5000);
}

//sends the home instruction to LCD
void lcdHome(){
	lcdSendInstr(0,0x02);
	_delay_us(3000);
}

//for turning ON/OFF the visibility of the cursor
void lcdShowCursor(bool action){
	if(action) lcdSendInstr(0,0x0E); //cursor + display on
	else lcdSendInstr(0,0x0C); //display on; cursor off
}

//puts the cursor on the desired position
void lcdSetCursor(uint8_t position){
	lcdHome();
	//increments the cursor until position is reached
	for (; position != 1; --position)
	{
		lcdSendInstr(0,0x14);	//instruction for incrementing the cursor
	}
}

//for sideway scrolling on LCD
void lcdScroll(bool dir, uint8_t n){
	//scroll until reached desired
	for (; n != 0; n--)
	{
		lcdSendInstr(0,0x18 | (dir<<2));	//scroll instruction
	}
}

//displays data on LCD
void lcdDispChar(char c){
	//special character
	if (c == '°') {	
		lcdSendInstr(1,0b11011111);
	}
	//general character
	else
		lcdSendInstr(1,c);
}

