#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <math.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

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

volatile uint8_t minuty, sekundy;
volatile uint16_t liczba7Seg;

volatile char znaki[4];

char cyfra[10] = { 0b1111110, 0b0110000, 0b1101101, 0b1111001, 0b0110011,
		0b1011011, 0b1011111, 0b1110000, 0b01111111, 0b1111011 };

//Inicjalizacja Timer1 do wywolywania przerwania z czêstotliwoœci¹ 2Hz
void TimerInit() {
	//Wybranie trybu pracy CTC z TOP OCR1A
	sbi(TCCR1B, WGM12); // uruchamia CTC

	//Wybranie dzielnika czestotliwosci
//	8MHz / dzielnik_cxestotliwosci
	// ustawiamy na 1024
	sbi(TCCR1B, CS10);
	sbi(TCCR1B, CS12);

	//Zapisanie do OCR1A wartosci odpowiadajacej 0,5s
	OCR1A = 3906;
	//Uruchomienie przerwania OCIE1A
	sbi(TIMSK, OCIE1A);
}

//Inicjalizacja portow do obs³ugi wyswietlacza 7 segmentowego
void seg7Init() {
	//Inicjalizacja kolumn
	for (uint8_t i = 0; i < 7; i++)
		sbi(DDRC, i);
	//Inicjalizacja segmentu

}

//Wyswietla na wyswietlaczu 7 segmentowym cyfre z argumentu
void seg7ShowCyfra(uint8_t cyfraDoWyswietlenia) {
	PORTC = cyfra[9];	//co to robi - wytlumaczyc prowadzacemu
}

int main() {
	TimerInit();
	seg7Init();

	sbi(DDRB, PB0);

	sei();
	//funkcja uruchamia globalne przerwania

	while (1) {
		for (uint8_t i = 0; i < 10; i++) {
			seg7ShowCyfra(i);
			_delay_ms(500);
		}
	}

	return 0;
}

//Funkcja uruchamiana z przerwaniem po przepelnieniu licznika w timer1
ISR(TIMER1_COMPA_vect) {
	tbi(PORTB, PB0);
}
