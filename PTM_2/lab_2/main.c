// Rafa³‚ Rzewucki 248926

#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <math.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

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

char linia1[LINIA_LENGTH], linia2[LINIA_LENGTH]; // zmienna pomocnicza do wyœwietlania tekstu
char tmpLinia1[LINIA_LENGTH], tmpLinia2[LINIA_LENGTH]; // zmienne pomocnicze do sprawdzania zmiany w wyœwietlaczu
int accumulator = 0; // zmienna pomocnicza
int tmpNum = 0;
char inputNum[3] = { '0', '0', '0' };
char inputChar = ' ', operationToPerform = ' ';

char keyboard[4][4] = {						// klawisze klawiatury
		{ '-', '+', 'c', '=' },				// -	+	c	=
				{ '9', '6', '3', '#' },   	// 9	6	3	#
				{ '8', '5', '2', '0' },   	// 8	5	2	0
				{ '7', '4', '1', '*' } 		// 7	4	1	*
		};   //

char getButton() {

	for (int col = 7; col >= 4; col--) {         	// petla po kolumnach
		sbi(PORTC, col);            				// wÅ‚Ä…czanie kolumn
		for (int row = 3; row >= 0; row--)  		// petla po wierszach
			if (bit_is_set(PINC, row)) {   			// jesli jest stan wyskoki
				while (bit_is_set(PINC, row)) // trzymaj dopÃ³ki klawisz jest wcisniÄ™ty
					_delay_ms(10);                  // czekaj 10 ms
				return keyboard[col - 4][row]; // wypluj numer wciÅ›niÄ™tego klawisza
			}
		cbi(PORTC, col);  							// wyÅ‚Ä…czanie kolumn
	}

	return ' ';						// jak nic nie wcisniete to zwroc spacje
}

void updateLcd() {
	short int print = 0; 									// drukowac czy nie

	sprintf(tmpLinia1, "Wynik: %d", accumulator); 		// generowanie linii 1
	sprintf(tmpLinia2, "%d", tmpNum);					// generowanie linii 2

	// sprawdŸ zmiany
	for (int i = 0; i < LINIA_LENGTH; i++) { // sprawdzanie czy coœ siê zmieni³o
		if (!print && (tmpLinia1[i] != linia1[i] || tmpLinia2[i] != linia2[i]))
			print = 1;									// Jeœli tak to drukuj
		linia1[i] = tmpLinia1[i];	// przepisywanie linii do globalnej pamiêci
		linia2[i] = tmpLinia2[i];
	}

	// jesli inne to printuj
	if (print) {
		LCD_Clear(); // czyszczenie wyœwietlacza
		LCD_GoTo(0, 0); // Przejœcie do pierwszej linijki
		LCD_WriteText(linia1); // wypisywanie tekstu
		LCD_GoTo(0, 1);
		LCD_WriteText(linia2); // wypisywanie tekstu
	}
}

int main() {
	LCD_Initalize();
	LCD_Home();

	short int licznik = 1;

	while (1) { // g³ówna pêtla programu

		if (!(licznik % 4)) // ¿eby nie updatowac LCD tak czêsto to da³em takie coœ
			updateLcd();

		inputChar = getButton(); 			// pobieranie wcisnietego klawisza

		switch (inputChar) {
		case ' ': // nic nie rób
		case '#': // nic nie rób
			break;
		case '+':
		case '-':
		case '*':
		case '=':
			if (operationToPerform == ' ') {
				accumulator = tmpNum; // jeœli to pocz¹tek liczenia to przepisz to do akumulatora
			} else if (operationToPerform == '+')
				accumulator += tmpNum;		// jeœli masz to dodac to to dodaj
			else if (operationToPerform == '-')
				accumulator -= tmpNum;		// jesli masz to odjac to odejmij
			else if (operationToPerform == '*')
				accumulator *= tmpNum;			// jesli ... to pomnó¿

			inputNum[0] = inputNum[1] = inputNum[2] = '0'; // kasowanie "rejestru ko³owego"
			operationToPerform = inputChar;	// zapisywanie operacji która bêdzie wykonana po wprowadzeniu liczby
			if (operationToPerform == '=')// "koñczenie" obliczeñ to wyzeruj wprowadzon¹ liczbê
				tmpNum = 0;

			break;
		case 'c':							// kasuj wszystkie wprowadzone dane
			tmpNum = 0;
			accumulator = 0;
			inputNum[0] = inputNum[1] = inputNum[2] = '0';
			operationToPerform = ' ';
			break;
		default:
			// wprowadzono liczbê
			if (operationToPerform == '=') {
				accumulator = 0;
				operationToPerform = ' ';
			}
			inputNum[0] = inputNum[1];	// przesuñ rejestr
			inputNum[1] = inputNum[2];	// przesuñ rejestr
			inputNum[2] = inputChar;	// dodaj cyfrê na wolne miejsce
			sscanf(inputNum, "%d", &tmpNum); // konwertuj tablicê charów na liczbê
			break;
		}

		_delay_ms(50); // czekaj
		licznik = !(licznik % 4) ? 0 : licznik + 1; // inkrementacja licznika do wywo³ania update'u LCD
	}

	return 0;
}
