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
	uint8_t mute = MUTE_OFF;

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
				setDisplayTime(TIMEOUT_AUDIO);
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
				setDisplayTime(TIMEOUT_AUDIO);
				break;
			}
			break;
		case CMD_BTN_2:
			if (mute == MUTE_OFF) {
				mute = MUTE_ON;
				dispMode = MODE_MUTE;
			} else {
				mute = MUTE_OFF;
				dispMode = MODE_VOLUME;
			}
			setDisplayTime(TIMEOUT_AUDIO);
			tda7448SetMute(mute);
			break;
		case CMD_BTN_3:
			switch (dispMode) {
			case MODE_VOLUME:
			case MODE_BALANCE:
			case MODE_FRONT:
			case MODE_CENTER:
				dispMode++;
				setDisplayTime(TIMEOUT_AUDIO);
				break;
			default:
				dispMode = MODE_VOLUME;
				setDisplayTime(TIMEOUT_AUDIO);
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
			case MODE_MUTE:
				dispMode = MODE_VOLUME;
			case MODE_VOLUME:
			case MODE_BALANCE:
			case MODE_FRONT:
			case MODE_CENTER:
			case MODE_SUBWOOFER:
				mute = MUTE_OFF;
				tda7448ChangeParam(TDA7448_SND_VOLUME + dispMode - MODE_VOLUME, encCnt);
				setDisplayTime(TIMEOUT_AUDIO);
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
		case MODE_MUTE:
			showMute();
			setDisplayTime(TIMEOUT_AUDIO);
			break;
		default:
			break;
		}
	}

	return 0;
}
