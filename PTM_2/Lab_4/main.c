// Rafał‚ Rzewucki 248926

#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <math.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include "HD44780.h"

#ifndef _BV
#define _BV(bit)				(1<<(bit))
#endif
#ifndef sbi
#define sbi(reg,bit)		reg |= (_BV(bit))
#endif

#ifndef cbi
#define cbi(reg,bit)		reg &= ~(_BV(bit))
#endif

#define     bit_is_set(sfr, bit)   (_SFR_BYTE(sfr) & _BV(bit))
#define     bit_is_clear(sfr, bit)   (!(_SFR_BYTE(sfr) & _BV(bit)))

#define LINIA_LENGTH 15

const uint8_t tab[10] PROGMEM = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
const uint8_t tab2[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

int main() {

	LCD_Initalize();
	LCD_Home();
	char text[20];

	volatile uint8_t zmienna1 = 0;
	volatile uint8_t zmienna2 = 0;

	while (1) {
		zmienna1 = pgm_read_word(&tab[1]);
		zmienna2 = tab2[2];

		LCD_Clear();
		LCD_GoTo(0, 0);
		sprintf(text, "Z1: %d", zmienna1);
		LCD_WriteText(text);
		LCD_GoTo(0, 1);
		sprintf(text, "Z2: %d", zmienna2);
		LCD_WriteText(text);

		_delay_ms(250);
	}
}
