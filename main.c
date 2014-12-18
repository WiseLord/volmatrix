#include <avr/interrupt.h>
#include <util/delay.h>

#include "matrix.h"
#include "tda7448.h"

int main(void)
{
	matrixInit();
	sei();

	matrixClear();

	uint8_t cmd = CMD_EMPTY;
	uint8_t dispMode = MODE_STANDBY;

	while(1) {
		cmd = getCmdBuf();

		/* Handle command */
		switch (cmd) {
		case CMD_BTN_0:
			switch (dispMode) {
			case MODE_VOLUME:
				decVolume();
				break;
			case MODE_BALANCE:
				decBalance();
				break;
			case MODE_FRONT:
				decFront();
				break;
			case MODE_CENTER:
				decCenter();
				break;
			case MODE_SUBWOOFER:
				decSubwoofer();
				break;
			}
			setDisplayTime(1000);
			break;
		case CMD_BTN_2:
			switch (dispMode) {
			case MODE_VOLUME:
				incVolume();
				break;
			case MODE_BALANCE:
				incBalance();
				break;
			case MODE_FRONT:
				incFront();
				break;
			case MODE_CENTER:
				incCenter();
				break;
			case MODE_SUBWOOFER:
				incSubwoofer();
				break;
			}
			setDisplayTime(1000);
			break;
		case CMD_BTN_5:
			switch (dispMode) {
			case MODE_VOLUME:
				dispMode = MODE_BALANCE;
				break;
			case MODE_BALANCE:
				dispMode = MODE_FRONT;
				break;
			case MODE_FRONT:
				dispMode = MODE_CENTER;
				break;
			case MODE_CENTER:
				dispMode = MODE_SUBWOOFER;
				break;
			default:
				dispMode = MODE_VOLUME;
				break;
			}
			setDisplayTime(2000);
			break;
		}

		/* Exid to default mode if timer expired */
		if (getDisplayTime() == 0) {
			dispMode = MODE_VOLUME;
		}

		/* Show things */
		switch (dispMode) {
		case MODE_VOLUME:
			showVolume(getVolume());
			break;
		case MODE_BALANCE:
			showBalance(getBalance());
			break;
		case MODE_FRONT:
			showFront(getFront());
			break;
		case MODE_CENTER:
			showCenter(getCenter());
			break;
		case MODE_SUBWOOFER:
			showSubwoofer(getSubwoofer());
			break;
		default:
			break;
		}
	}

	return 0;
}
