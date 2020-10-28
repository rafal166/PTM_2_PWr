#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#ifndef _BV
#define _BV(bit)				(1<<(bit))
#endif
#ifndef sbi // 1
#define sbi(reg,bit)		reg |= (_BV(bit))
#endif

#ifndef cbi // 0
#define cbi(reg,bit)		reg &= ~(_BV(bit))
#endif

#ifndef tbi // 0
#define tbi(reg,bit)		reg ^= (_BV(bit))
#endif

#define DELAY 500

// DDRD Data Direction dla portu D
int main() {
	sbi(PORTC, PC6);
	sbi(DDRD, 0); // ustawianie portu jako wyjœcie.

	while (1) { // pêtla gwarantuj¹ca zapêtlanie siê programu
		if (bit_is_clear(PINC, PC6)) {
			sbi(PORTD, 0); // ustawianie stanu portu na 1
			_delay_ms(DELAY);
		} else {
			cbi(PORTD, 0);
			_delay_ms(DELAY);
		} // Czekanie

		_delay_ms(DELAY);
	}
	return 0;
}
