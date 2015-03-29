#include <avr/interrupt.h>
#include <util/delay.h>

#include "matrix.h"
#include "tda7448.h"

static uint8_t mute = MUTE_ON;
static uint8_t stby = STBY_ON;
static uint8_t dispMode = MODE_STANDBY;

static void powerOn(void)
{
	stby = STBY_OFF;
	mute = MUTE_OFF;
	tda7448SetMute(mute);
	dispMode = MODE_VOLUME;
	setDisplayTime(TIMEOUT_AUDIO);
}

static void powerOff(void)
{
	stby = STBY_ON;
	mute = MUTE_ON;
	tda7448SetMute(mute);
	dispMode = MODE_STANDBY;
	setDisplayTime(TIMEOUT_STBY);
	matrixClear();
}

int main(void)
{
	matrixInit();
	matrixClear();
	sei();

	_delay_ms(100);

	int8_t encCnt = 0;
	uint8_t cmd = CMD_EMPTY;

	tda7448LoadParams();
	powerOff();

	while(1) {
		encCnt = getEncoder();
		cmd = getCmdBuf();

		/* Don't handle commands in standby mode */
		if (dispMode == MODE_STANDBY) {
			encCnt = 0;
			if (cmd != CMD_BTN_1 && cmd != CMD_BTN_3_LONG)
				cmd = CMD_EMPTY;
		}

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
		case CMD_BTN_1:
		case CMD_BTN_3_LONG:
			if (dispMode == MODE_STANDBY) {
				powerOn();
			} else {
				powerOff();
				tda7448SaveParams();
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
		if (getDisplayTime() == 0 && dispMode != MODE_STANDBY) {
			if (mute == MUTE_ON)
				dispMode = MODE_MUTE;
			else
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
			break;
		case MODE_STANDBY:
			break;
		default:
			break;
		}
	}

	return 0;
}
