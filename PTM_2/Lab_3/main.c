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
const float MAX_VOLTAGE = 5.00;

char linia1[LINIA_LENGTH], linia2[LINIA_LENGTH]; // zmienna pomocnicza do wyświetlania tekstu
char tmpLinia1[LINIA_LENGTH], tmpLinia2[LINIA_LENGTH]; // zmienne pomocnicze do sprawdzania zmiany w wyświetlaczu

uint16_t analog_voltage[2];
uint16_t real_voltage[2];
uint8_t precent_voltage[2];

// obsługa wyświetlacza
void updateLcd() {
	short int print = 0; 									// drukowac czy nie
	char tmp[15];

	dtostrf(real_voltage[0] / 100.00, 1, 2, tmp); // zamiana float na tablicę charów
	sprintf(tmpLinia1, "%s - %d%%", tmp, precent_voltage[0]); // generowanie linii 1
	dtostrf(real_voltage[1] / 100.00, 1, 2, tmp);
	sprintf(tmpLinia2, "%s - %d%%", tmp, precent_voltage[1]); // generowanie linii 2

	// sprawdź zmiany
	for (int i = 0; i < LINIA_LENGTH; i++) { // sprawdzanie czy coś się zmieniło
		if (!print && (tmpLinia1[i] != linia1[i] || tmpLinia2[i] != linia2[i]))
			print = 1;									// Jeśli tak to drukuj
		linia1[i] = tmpLinia1[i];	// przepisywanie linii do globalnej pamięci
		linia2[i] = tmpLinia2[i];
	}

	// jesli inne to printuj
	if (print) {
		LCD_Clear(); // czyszczenie wyświetlacza
		LCD_GoTo(0, 0); // Przejście do pierwszej linijki
		LCD_WriteText(linia1); // wypisywanie tekstu
		LCD_GoTo(0, 1);
		LCD_WriteText(linia2); // wypisywanie tekstu
	}
}

// przetwornik AD
void ADC_init() {

	sbi(ADMUX, REFS0); // ustawianie napiêcia referencyjnego AVCC

	sbi(ADCSRA, ADPS0); // ustawianie podzielnika czêstotliwoœci na 2
	sbi(ADCSRA, ADEN); // uruchomineie uk³adu przetwornika
}

int analogToVoltage(uint16_t current) { // * 100 żeby bez przecinka była
	return (MAX_VOLTAGE * current / 1022.00) * 100;
}

uint8_t analogToPercentVoltage(uint16_t current) {
	return (current / 1022.00) * 100;
}

uint16_t ADC_10bit() {
	sbi(ADCSRA, ADSC); // start
	loop_until_bit_is_set(ADCSRA, ADSC); // czekaj na koniec pomiaru
	return ADC;
}

void dwupolozeniowy(uint16_t wartoscZadana, uint16_t wartoscRzeczywista) {
	if (wartoscRzeczywista < wartoscZadana)
		sbi(PORTD, 0);
	else if (wartoscRzeczywista > wartoscZadana)
		cbi(PORTD, 0);
}

void dwupolozeniowy_z_histereza(uint16_t wartoscZadana,
		uint16_t wartoscRzeczywista, uint8_t histereza) {
	if (wartoscRzeczywista + histereza < wartoscZadana)
		sbi(PORTD, 1);
	else if (wartoscRzeczywista - histereza > wartoscZadana)
		cbi(PORTD, 1);
}

void trojpolozeniowy(uint16_t wartoscZadana, uint16_t wartoscRzeczywista) {
	uint8_t neutral = 100;
	if (wartoscRzeczywista + neutral < wartoscZadana) {
		sbi(PORTD, 2);
		cbi(PORTD, 3);
	} else if (wartoscRzeczywista - neutral > wartoscZadana) {
		cbi(PORTD, 2);
		sbi(PORTD, 3);
	} else {
		cbi(PORTD, 2);
		cbi(PORTD, 3);
	}
}
int main() {
	short int licznik = 1;
	DDRC = 0b00001111;
	LCD_Initalize();
	LCD_Home();
	ADC_init(); // inicjacja przetwornika A/D

	while (1) { // główna pętla programu

		if (!(licznik % 4)) // żeby nie updatowac LCD tak często to dałem takie coś
			updateLcd();

		//pomiar
		cbi(ADMUX, MUX0); // kanał 0
		analog_voltage[0] = ADC_10bit();
		_delay_ms(1); // bez tego nie działa
		sbi(ADMUX, MUX0); // kanał 1
		analog_voltage[1] = ADC_10bit();

		for (uint8_t i = 0; i < 2; i++) {
			real_voltage[i] = analogToVoltage(analog_voltage[i]);
			precent_voltage[i] = analogToPercentVoltage(analog_voltage[i]);
		}

		dwupolozeniowy(real_voltage[0], real_voltage[1]);
		dwupolozeniowy_z_histereza(real_voltage[0], real_voltage[1], 50);
		trojpolozeniowy(real_voltage[0], real_voltage[1]);

		_delay_ms(50); // czekaj
		licznik = !(licznik % 4) ? 0 : licznik + 1; // inkrementacja licznika do wywołania update'u LCD
	}

	return 0;
}
