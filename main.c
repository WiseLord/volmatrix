#include <avr/interrupt.h>
#include <util/delay.h>

#include "matrix.h"
#include "tda7448.h"

int main(void)
{
	matrixInit();
	matrixClear();
	sei();

	_delay_ms(100);

	int8_t encCnt = 0;
	uint8_t cmd = CMD_EMPTY;
	uint8_t dispMode = MODE_STANDBY;

	while(1) {
		encCnt = getEncoder();
		cmd = getCmdBuf();

		/* Handle command */
		switch (cmd) {
		case CMD_RC5_VOL_DOWN:
			switch (dispMode) {
			case MODE_VOLUME:
			case MODE_BALANCE:
			case MODE_FRONT:
			case MODE_CENTER:
			case MODE_SUBWOOFER:
				tda7448ChangeParam(TDA7448_SND_VOLUME + dispMode - MODE_VOLUME, -1);
				setDisplayTime(1000);
				break;
			}
			break;
		case CMD_RC5_VOL_UP:
			switch (dispMode) {
			case MODE_VOLUME:
			case MODE_BALANCE:
			case MODE_FRONT:
			case MODE_CENTER:
			case MODE_SUBWOOFER:
				tda7448ChangeParam(TDA7448_SND_VOLUME + dispMode - MODE_VOLUME, +1);
				setDisplayTime(1000);
				break;
			}
			break;
		case CMD_BTN_3:
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

		/* Emulate RC5 VOL_UP/VOL_DOWN as encoder actions */
		if (cmd == CMD_RC5_VOL_UP)
			encCnt++;
		if (cmd == CMD_RC5_VOL_DOWN)
			encCnt--;

		/* Handle encoder */
		if (encCnt) {
			switch (dispMode) {
			case MODE_STANDBY:
				break;
			case MODE_VOLUME:
			case MODE_BALANCE:
			case MODE_FRONT:
			case MODE_CENTER:
			case MODE_SUBWOOFER:
				tda7448ChangeParam(TDA7448_SND_VOLUME + dispMode - MODE_VOLUME, encCnt);
				setDisplayTime(1000);
				break;
			default:
				break;
			}
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
