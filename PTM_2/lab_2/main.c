// Rafa�� Rzewucki 248926

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

char linia1[LINIA_LENGTH], linia2[LINIA_LENGTH]; // zmienna pomocnicza do wy�wietlania tekstu
char tmpLinia1[LINIA_LENGTH], tmpLinia2[LINIA_LENGTH]; // zmienne pomocnicze do sprawdzania zmiany w wy�wietlaczu
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
		sbi(PORTC, col);            				// włączanie kolumn
		for (int row = 3; row >= 0; row--)  		// petla po wierszach
			if (bit_is_set(PINC, row)) {   			// jesli jest stan wyskoki
				while (bit_is_set(PINC, row)) // trzymaj dopóki klawisz jest wcisnięty
					_delay_ms(10);                  // czekaj 10 ms
				return keyboard[col - 4][row]; // wypluj numer wciśniętego klawisza
			}
		cbi(PORTC, col);  							// wyłączanie kolumn
	}

	return ' ';						// jak nic nie wcisniete to zwroc spacje
}

void updateLcd() {
	short int print = 0; 									// drukowac czy nie

	sprintf(tmpLinia1, "Wynik: %d", accumulator); 		// generowanie linii 1
	sprintf(tmpLinia2, "%d", tmpNum);					// generowanie linii 2

	// sprawd� zmiany
	for (int i = 0; i < LINIA_LENGTH; i++) { // sprawdzanie czy co� si� zmieni�o
		if (!print && (tmpLinia1[i] != linia1[i] || tmpLinia2[i] != linia2[i]))
			print = 1;									// Je�li tak to drukuj
		linia1[i] = tmpLinia1[i];	// przepisywanie linii do globalnej pami�ci
		linia2[i] = tmpLinia2[i];
	}

	// jesli inne to printuj
	if (print) {
		LCD_Clear(); // czyszczenie wy�wietlacza
		LCD_GoTo(0, 0); // Przej�cie do pierwszej linijki
		LCD_WriteText(linia1); // wypisywanie tekstu
		LCD_GoTo(0, 1);
		LCD_WriteText(linia2); // wypisywanie tekstu
	}
}

int main() {
	LCD_Initalize();
	LCD_Home();

	short int licznik = 1;

	while (1) { // g��wna p�tla programu

		if (!(licznik % 4)) // �eby nie updatowac LCD tak cz�sto to da�em takie co�
			updateLcd();

		inputChar = getButton(); 			// pobieranie wcisnietego klawisza

		switch (inputChar) {
		case ' ': // nic nie r�b
		case '#': // nic nie r�b
			break;
		case '+':
		case '-':
		case '*':
		case '=':
			if (operationToPerform == ' ') {
				accumulator = tmpNum; // je�li to pocz�tek liczenia to przepisz to do akumulatora
			} else if (operationToPerform == '+')
				accumulator += tmpNum;		// je�li masz to dodac to to dodaj
			else if (operationToPerform == '-')
				accumulator -= tmpNum;		// jesli masz to odjac to odejmij
			else if (operationToPerform == '*')
				accumulator *= tmpNum;			// jesli ... to pomn�

			inputNum[0] = inputNum[1] = inputNum[2] = '0'; // kasowanie "rejestru ko�owego"
			operationToPerform = inputChar;	// zapisywanie operacji kt�ra b�dzie wykonana po wprowadzeniu liczby
			if (operationToPerform == '=')// "ko�czenie" oblicze� to wyzeruj wprowadzon� liczb�
				tmpNum = 0;

			break;
		case 'c':							// kasuj wszystkie wprowadzone dane
			tmpNum = 0;
			accumulator = 0;
			inputNum[0] = inputNum[1] = inputNum[2] = '0';
			operationToPerform = ' ';
			break;
		default:
			// wprowadzono liczb�
			if (operationToPerform == '=') {
				accumulator = 0;
				operationToPerform = ' ';
			}
			inputNum[0] = inputNum[1];	// przesu� rejestr
			inputNum[1] = inputNum[2];	// przesu� rejestr
			inputNum[2] = inputChar;	// dodaj cyfr� na wolne miejsce
			sscanf(inputNum, "%d", &tmpNum); // konwertuj tablic� char�w na liczb�
			break;
		}

		_delay_ms(50); // czekaj
		licznik = !(licznik % 4) ? 0 : licznik + 1; // inkrementacja licznika do wywo�ania update'u LCD
	}

	return 0;
}
