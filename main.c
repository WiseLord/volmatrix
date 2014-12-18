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
			case MODE_BALANCE:
			case MODE_FRONT:
			case MODE_CENTER:
			case MODE_SUBWOOFER:
				tda7448DecParam(TDA7448_SND_VOLUME + dispMode - MODE_VOLUME);
				setDisplayTime(1000);
				break;
			}
			break;
		case CMD_BTN_2:
			switch (dispMode) {
			case MODE_VOLUME:
			case MODE_BALANCE:
			case MODE_FRONT:
			case MODE_CENTER:
			case MODE_SUBWOOFER:
				tda7448IncParam(TDA7448_SND_VOLUME + dispMode - MODE_VOLUME);
				setDisplayTime(1000);
				break;
			}
			break;
		case CMD_BTN_5:
			switch (dispMode) {
			case MODE_VOLUME:
			case MODE_BALANCE:
			case MODE_FRONT:
			case MODE_CENTER:
				dispMode++;
				setDisplayTime(2000);
				break;
			case MODE_SUBWOOFER:
				dispMode = MODE_VOLUME;
				setDisplayTime(2000);
				break;
			}
			break;
		}

		/* Exid to default mode if timer expired */
		if (getDisplayTime() == 0) {
			dispMode = MODE_VOLUME;
		}

		/* Show things */
		switch (dispMode) {
		case MODE_VOLUME:
		case MODE_BALANCE:
		case MODE_FRONT:
		case MODE_CENTER:
		case MODE_SUBWOOFER:
			showAudio(TDA7448_SND_VOLUME + dispMode - MODE_VOLUME);
			break;
		default:
			break;
		}
	}

	return 0;
}
