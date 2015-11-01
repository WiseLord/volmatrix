#include <avr/interrupt.h>
#include <util/delay.h>

#include "matrix.h"
#include "audio/audioproc.h"
#include "rc5.h"

#define STBY_ON						1
#define STBY_OFF					0

static uint8_t mute = MUTE_ON;
static uint8_t stby = STBY_ON;
static uint8_t dispMode = MODE_STANDBY;

static void powerOn(void)
{
	stby = STBY_OFF;
	mute = MUTE_OFF;
	sndSetMute(mute);
	dispMode = MODE_SND_VOLUME;
	setDisplayTime(TIMEOUT_AUDIO);
}

static void powerOff(void)
{
	stby = STBY_ON;
	mute = MUTE_ON;
	sndSetMute(mute);
	sndPowerOff();
	dispMode = MODE_STANDBY;
	setDisplayTime(TIMEOUT_STBY);
	matrixFadeOff();
}

int main(void)
{
	rc5Init();
	sndInit();
	matrixInit();
	sei();

	matrixFill(0xFF);
	_delay_ms(100);
	powerOff();
	sndInit();

	int8_t encCnt = 0;
	uint8_t cmd = CMD_EMPTY;
	uint8_t input;

	while(1) {
		encCnt = getEncoder();
		cmd = getCmdBuf();

		/* Don't handle commands in standby mode */
		if (dispMode == MODE_STANDBY) {
			encCnt = 0;
			if (cmd != CMD_RC5_STBY && cmd != CMD_BTN_1 && cmd != CMD_BTN_3_LONG)
				cmd = CMD_EMPTY;
		}

		/* Handle command */
		switch (cmd) {
		case CMD_RC5_STBY:
		case CMD_BTN_1:
			if (dispMode == MODE_STANDBY)
				powerOn();
			else
				powerOff();
			break;
		case CMD_RC5_MUTE:
		case CMD_BTN_2:
			if (mute == MUTE_OFF) {
				mute = MUTE_ON;
				dispMode = MODE_MUTE;
			} else {
				mute = MUTE_OFF;
				dispMode = MODE_SND_VOLUME;
			}
			setDisplayTime(TIMEOUT_AUDIO);
			sndSetMute(mute);
			break;
		case CMD_RC5_MENU:
		case CMD_BTN_3:
			sndNextParam(&dispMode);
			setDisplayTime(TIMEOUT_AUDIO);
			break;
		case CMD_RC5_RED:
		case CMD_RC5_GREEN:
		case CMD_RC5_YELLOW:
		case CMD_RC5_BLUE:
			if (cmd == CMD_RC5_BLUE && sndInputCnt() < 4) {
				if (sndGetLoudness()) {
					sndSetLoudness(LOUDNESS_OFF);
					dispMode = MODE_SND_VOLUME;
				} else {
					sndSetLoudness(LOUDNESS_ON);
					dispMode = MODE_LOUDNESS;
				}
			} else {
				sndSetInput(cmd - CMD_RC5_RED);
				dispMode = MODE_SND_GAIN0 + (cmd - CMD_RC5_RED);
			}
			setDisplayTime(TIMEOUT_AUDIO);
			break;
		case CMD_BTN_2_LONG:
			if (sndGetLoudness()) {
				sndSetLoudness(LOUDNESS_OFF);
				dispMode = MODE_SND_VOLUME;
			} else {
				sndSetLoudness(LOUDNESS_ON);
				dispMode = MODE_LOUDNESS;
				setDisplayTime(TIMEOUT_AUDIO);
			}
			break;
		case CMD_RC5_NEXT:
		case CMD_BTN_3_LONG:
			input = sndGetInput();
			if (dispMode >= MODE_SND_GAIN0 && dispMode <= MODE_SND_GAIN3)
				input++;
			if (input >= sndInputCnt())
				input = 0;
			sndSetInput(input);
			dispMode = MODE_SND_GAIN0 + input;
			setDisplayTime(TIMEOUT_AUDIO);
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
				dispMode = MODE_SND_VOLUME;
			default:
				sndSetMute(MUTE_OFF);
				sndChangeParam(dispMode, encCnt);
				setDisplayTime(TIMEOUT_AUDIO);
				break;
			}
		}

		/* Exid to default mode if timer expired */
		if (getDisplayTime() == 0 && dispMode != MODE_STANDBY) {
			if (mute == MUTE_ON)
				dispMode = MODE_MUTE;
			else
				dispMode = MODE_SND_VOLUME;
		}

		/* Show things */
		switch (dispMode) {
		case MODE_STANDBY:
			break;
		case MODE_MUTE:
			showMute();
			break;
		case MODE_LOUDNESS:
			showLoudness();
			break;
		default:
			showSndParam(dispMode, ICON_NATIVE);
			break;
		}
	}

	return 0;
}
