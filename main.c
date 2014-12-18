#include <avr/interrupt.h>
#include <util/delay.h>

#include "matrix.h"

int main(void)
{
	matrixInit();
	sei();

	matrixClear();

	uint8_t cmd = CMD_EMPTY;

	while(1) {
		cmd = getCmdBuf();

		switch (cmd) {
		case CMD_BTN_0:
			showBalance(4);
			_delay_ms(500);
			break;
		case CMD_BTN_2:
			showFront(-5);
			_delay_ms(500);
			break;
		case CMD_BTN_0_LONG:
			showSubwoofer(-4);
			_delay_ms(500);
			break;
		case CMD_BTN_2_LONG:
			showCenter(-6);
			_delay_ms(500);
			break;
		default:
			showVolume(-50);
			break;
		}
	}

	return 0;
}
