#include <avr/interrupt.h>
#include <util/delay.h>

#include "matrix.h"

int main(void)
{
	matrixInit();
	sei();

	matrixClear();

	int8_t i;

	while (1) {
		for (i = -8; i <= 8; i++) {
			showBalance(i);
			_delay_ms(200);
		}
		for (i = -8; i <= 8; i++) {
			showFront(i);
			_delay_ms(200);
		}
		for (i = -16; i <= 0; i++) {
			showCenter(i);
			_delay_ms(200);
		}
		for (i = -16; i <= 0; i++) {
			showSubwoofer(i);
			_delay_ms(200);
		}
		for (i = -79; i <= 0; i++) {
			showVolume(i);
			_delay_ms(200);
		}
	}

	return 0;
}
