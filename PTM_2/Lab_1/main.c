// Rafał Rzewucki 248926

#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <math.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

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

void setLED(uint8_t led, uint8_t stan) {
// toggle diody
	if (stan)
		sbi(PORTD, led - 1);
	else
		cbi(PORTD, led - 1);
}

int getButton() {

	int keyboard[4][4] = {						// klawisze klawiatury
			{ 4, 8, 12, 16 },		// -	+	c	=
					{ 3, 7, 11, 15 },   // 9	6	3	#
					{ 2, 6, 10, 14 },   // 8	5	2	0
					{ 1, 5, 9, 13 } 	// 7	4	1	*
			};   //
	for (int col = 7; col >= 4; col--) {         	// petla po kolumnach
		sbi(PORTC, col);            				// włączanie kolumn
		for (int row = 3; row >= 0; row--)  		// petla po wierszach
			if (bit_is_set(PINC, row)) {   			// jesli jest stan wyskoki
				while (bit_is_set(PINC, row)) // trzymaj dopóki klawisz jest wcisnięty
					_delay_ms(10);                  // czekaj 10 ms
				return keyboard[col - 4][row]; // wypluj numer wciśniętego klawisza
			}
		cbi(PORTC, col);  							// wyłączanie kolumn
	}

	return -1;								// jak nic nie wciśnięte to zwróc -1
}

int main() {
	int guzik;

	for (int i = 0; i < 8; i++)	//inicjalizacja led
		sbi(DDRD, i);

	for (int i = 1; i < 9; i++)	// zgaszenie wszystkich led
		setLED(i, 0);

	while (1) {

		guzik = getButton();	// Pobranie wartości z klawiatury

		if (guzik > 0 && guzik < 9) { // jeśli wartośc 1-8 => zapal tego leda
			if (bit_is_set(PORTD, guzik - 1)) // jesli zapalona
				setLED(guzik, 0); // zgas led

			else
				// zapal led
				setLED(guzik, 1);
		}

		if (guzik == 13)   // jesli wcisnięta *
			for (int w = 1; w < 9; w++)   // zapala all led
				setLED(w, 1);

		if (guzik == 15)   // jesli wciśniety #
			for (int i = 1; i < 9; i++)  // gasi all led
				setLED(i, 0);

		_delay_ms(100);
	}
}
