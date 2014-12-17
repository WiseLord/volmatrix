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
		for (i = -79; i <= 0; i++) {
			matrixSetPos(5);
			matrixShowNumber(i);
			matrixShowVolBar(i);
			_delay_ms(500);
		}
	}

	return 0;
}

