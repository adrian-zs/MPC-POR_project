#ifndef LCD_H_
#define LCD_H_

#include <stdbool.h>
#include <stdio.h>

static int lcd_putchar(char c, FILE *stream);									//needed for fprintf setup
static FILE lcd = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);		//creates "file" to use LCD with fprintf

void lcdSendInstr(bool rs, uint8_t instr);
void lcdInit();
void lcdClear();
void lcdHome();
void lcdShowCursor(bool action);
void lcdSetCursor(uint8_t position);
void lcdScroll(bool dir, uint8_t n);
void lcdDispChar(char c);

//for writing characters on LCD with fprintf
static int lcd_putchar(char t, FILE *stream)
{
	if (t == '\n')
	lcd_putchar('\r', stream);
	lcdDispChar(t);
	return 0;
}
#endif

