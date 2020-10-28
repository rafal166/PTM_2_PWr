#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <math.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "HD44780.h"

#ifndef _BV
#define _BV(bit)				(1<<(bit))
#endif

#ifndef inb
#define	inb(addr)			(addr)
#endif

#ifndef outb
#define	outb(addr, data)	addr = (data)
#endif

#ifndef sbi
#define sbi(reg,bit)		reg |= (_BV(bit))
#endif

#ifndef cbi
#define cbi(reg,bit)		reg &= ~(_BV(bit))
#endif

#ifndef tbi
#define tbi(reg,bit)		reg ^= (_BV(bit))
#endif

#define MAX_VOLTAGE = 5;
/*
 *  Gotowe zaimplementowane:
 #define 	bit_is_set(sfr, bit)   (_SFR_BYTE(sfr) & _BV(bit))
 #define 	bit_is_clear(sfr, bit)   (!(_SFR_BYTE(sfr) & _BV(bit)))
 #define 	loop_until_bit_is_set(sfr, bit)   do { } while (bit_is_clear(sfr, bit))
 #define 	loop_until_bit_is_clear(sfr, bit)   do { } while (bit_is_set(sfr, bit))

 */

// MIN/MAX/ABS macros
#define MIN(a,b)			((a<b)?(a):(b))
#define MAX(a,b)			((a>b)?(a):(b))
#define ABS(x)				((x>0)?(x):(-x))
#define PIN_UP 1
#define PIN_DOWN 2
#define PIN_OK 3
#define PIN_X 4
#define PIN_LED1 2
#define PIN_LED2 3
#define PIN_LED3 6

volatile int main_counter = 0.0;
char cyfra[11] = { 0b1111110, 0b0110000, 0b1101101, 0b1111001, 0b0110011,
		0b1011011, 0b1011111, 0b1110000, 0b01111111, 0b1111011, 0b0000001 };

//Inicjalizacja portow do obs³ugi wyswietlacza 7 segmentowego
void seg7Init() {
	//Inicjalizacja kolumn
	for (uint8_t i = 0; i < 7; i++)
		sbi(DDRC, i);
	//Inicjalizacja segmentu

}

//Wyswietla na wyswietlaczu 7 segmentowym cyfre z argumentu
void seg7ShowCyfra(uint8_t cyfraDoWyswietlenia) {
	if (cyfraDoWyswietlenia > 9)
		cyfraDoWyswietlenia = 10;
	PORTC = cyfra[cyfraDoWyswietlenia];	//co to robi - wytlumaczyc prowadzacemu
}

void Timer1Init() {
	//Wybranie trybu pracy CTC z TOP OCR1A
	sbi(TCCR1B, WGM12); // uruchamia CTC

	//Wybranie dzielnika czestotliwosci
//	8MHz / dzielnik_cxestotliwosci
	// ustawiamy na 1024
	sbi(TCCR1B, CS10);
	sbi(TCCR1B, CS12);

	//Zapisanie do OCR1A wartosci odpowiadajacej 0,5s
	OCR1A = 781;
}
void ADC_init() {

	sbi(ADMUX, REFS0); // ustawianie napięcia referencyjnego AVCC

	sbi(ADCSRA, ADPS0); // ustawianie podzielnika częstotliwości na 2
	sbi(ADCSRA, ADEN); // uruchomineie układu przetwornika
}

int convertToVoltage(uint16_t current) {
	return (5.00 * current / 1023.00) * 100.00;

}

uint16_t ADC_10bit() {
	sbi(ADCSRA, ADSC); // rozpczęcie pomiaru
	loop_until_bit_is_set(ADCSRA, ADSC); // pętla aż poiar się nie skończy
	return ADC; // zwraca pomiar
}

void info_start() {

	int licznik = 0;
	LCD_Clear(); // czysci wyswietlacz
	LCD_GoTo(0, 0); // przechodzi do pierwszej linijki
	LCD_WriteText("Program PTM 2020"); // wyswietla zadany ciąg znaków
	LCD_GoTo(0, 1);
	LCD_WriteText("248931, 248926"); // wyswietla zadany ciąg znaków
	while (licznik < 4000) {
		if (bit_is_clear(PINA, PIN_X))
			break;
		licznik += 50;
		_delay_ms(50);
	}
}

int liczby() {
	char text[20];
	int liczba = 0;
	while (1) {

		if (bit_is_clear(PINA, PIN_UP)) {
			while (bit_is_clear(PINA, PIN_UP))
				_delay_ms(10);
			liczba++;
		}

		if (bit_is_clear(PINA, PIN_DOWN)) {
			while (bit_is_clear(PINA, PIN_DOWN))
				_delay_ms(10);
			liczba--;
		}
		if (liczba > 50)
			liczba = 0;
		if (liczba < 0)
			liczba = 50;

		seg7ShowCyfra(liczba);
		LCD_Clear(); // czysci wyswietlacz
		LCD_GoTo(0, 0); // przechodzi do pierwszej linijki

		sprintf(text, " Liczba : %d", liczba);
		LCD_WriteText(text); // wyswietla zadany ciąg znaków

		if (liczba % 2 == 0) {
			sbi(PORTD, PIN_LED1);
			_delay_ms(20);
			cbi(PORTD, PIN_LED2);
		} else {
			sbi(PORTD, PIN_LED2);
			cbi(PORTD, PIN_LED1);
		}

		if (liczba == 2 || liczba == 3 || liczba == 5 || liczba == 7
				|| liczba == 11 || liczba == 13 || liczba == 17 || liczba == 19
				|| liczba == 23 || liczba == 29 || liczba == 31 || liczba == 37
				|| liczba == 41 || liczba == 43 || liczba == 47)
			sbi(PORTD, PIN_LED3);
		else
			cbi(PORTD, PIN_LED3);

		if (bit_is_clear(PINA, PIN_X))
			return 0;
		_delay_ms(50);
	}
}

int stoper() {
	char text[20];
	main_counter = 0;
	while (1) {
		LCD_Clear(); // czysci wyswietlacz
		LCD_GoTo(0, 0); // przechodzi do pierwszej linijki

		sprintf(text, "Stoper : %d.%d0", main_counter / 10, main_counter % 10);
		LCD_WriteText(text); // wyswietla zadany ciąg znaków

		if (main_counter % 9 == 0) // dioda
			tbi(PORTD, PIN_LED1);

		if (bit_is_clear(PINA, PIN_OK)) {
			//Uruchomienie przerwania OCIE1A
			sbi(TIMSK, OCIE1A);

			while (bit_is_clear(PINA, PIN_OK))
				_delay_ms(20);

			if (bit_is_clear(PINA, PIN_OK)) {
				_delay_ms(20);
				cbi(TIMSK, OCIE1A);
			}
			while (bit_is_set(PINA, PIN_OK))
				_delay_ms(30);
			main_counter = 0;
		}

		if (bit_is_clear(PINA, PIN_X))
			return 0;
		_delay_ms(50);
	}
}

int zegar() {
	char text[20];
	main_counter = 0;
	int minuty = 21;
	int sekundy = 0;

	while (1) {
		sbi(TIMSK, OCIE1A);
		LCD_Clear(); // czysci wyswietlacz
		LCD_GoTo(0, 0); // przechodzi do pierwszej linijki

		sekundy = main_counter / 10;
		if (sekundy > 59) {
			minuty++;
			sekundy = main_counter = 0;
		}

		if (main_counter % 10 == 0) {
			sbi(PORTD, PIN_LED3);
			_delay_ms(200);
			cbi(PORTD, PIN_LED3);
		}

		sprintf(text, "Zegar : %d.%d", minuty, main_counter / 10);
		LCD_WriteText(text); // wyswietla zadany ciąg znaków

		//	if (main_counter % 9 == 0) // dioda
		//	tbi(PORTD, PIN_LED1);

		seg7ShowCyfra(sekundy);

		if (bit_is_clear(PINA, PIN_X))
			return 0;
		_delay_ms(50);
	}
	cbi(TIMSK, OCIE1A);
}

int miernik() {
	char text[20]; // zmienna pomocnicza
	int votage;
	uint16_t odczyt;
	ADC_init(); // inicjacja przetwornika A/D
	sbi(DDRC, 3);

	while (1) {
		odczyt = ADC_10bit();
		votage = convertToVoltage(odczyt);
		LCD_Clear(); // czysci wyswietlacz

		LCD_GoTo(0, 0); // przechodzi do pierwszej linijki
		sprintf(text, "Odczyt: %d", odczyt); // zapisuje nowy ciąg znaków do wyświetlenia
		LCD_WriteText(text); // wyswietla zadany ciąg znaków

		LCD_GoTo(0, 1); // przechodzi do pierwszej linijki
		sprintf(text, "Napiecie: %d.%dV", votage/100, votage %100); // zapisuje nowy ciąg znaków do wyświetlenia
		LCD_WriteText(text); // wyswietla zadany ciąg znaków

		if (bit_is_clear(PINA, PIN_X))
			return 0;
		_delay_ms(500);
	}
}

void menu_engine() {
	static int menu = 0;

// określanie pozycji menu
	if (bit_is_clear(PINA, 1)) {
		while (bit_is_clear(PINA, 1))
			_delay_ms(10);
		menu++;
	}
	if (bit_is_clear(PINA, 2)) {
		while (bit_is_clear(PINA, 2))
			_delay_ms(10);
		menu--;
	}
	if (menu > 4)
		menu = 0;
	if (menu < 0)
		menu = 4;

// wyświetlanie menu
	LCD_Clear();
	LCD_GoTo(0, 0);
	switch (menu) {
	case 0:
		LCD_WriteText("Wyswietl Info");
		if (bit_is_clear(PINA, PIN_OK))
			info_start();
		break;
	case 1:
		//default:
		LCD_WriteText("Wyswietl Liczby");
		if (bit_is_clear(PINA, PIN_OK))
			liczby();
		break;
	case 2:
		//default:
		LCD_WriteText("Stoper");
		if (bit_is_clear(PINA, PIN_OK))
			stoper();
		break;
	case 3:
		//default:
		LCD_WriteText("Zegar");
		if (bit_is_clear(PINA, PIN_OK))
			zegar();
		break;
	case 4:
		//default:
		LCD_WriteText("Miernik");
		if (bit_is_clear(PINA, PIN_OK))
			miernik();
		break;
	}

}

int main() {

// inicjalizacja przyciskow
	for (int i = 1; i < 5; i++)
		sbi(PORTA, i);
	// DIODKI
	sbi(DDRD, 2);
	sbi(DDRD, 3);
	sbi(DDRD, 6);

	LCD_Initalize();
	LCD_Home();
	seg7Init();
	// odpala timer1
	Timer1Init();
	sei();
	info_start();

//	char text[20]; // zmienna pomocnicza
//	int votage;
//	ADC_init(); // inicjacja przetwornika A/D
//	sbi(DDRC, 3);

	while (1) {

		menu_engine();

		_delay_ms(100);
	}
}

ISR(TIMER1_COMPA_vect) {
	main_counter += 1;
}
