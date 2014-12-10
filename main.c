#include <avr/interrupt.h>

#include "matrix.h"


int main(void)
{
	matrixInit();
	sei();

	while (1) {
	}

	return 0;
}

