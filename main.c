#include <avr/interrupt.h>
#include <util/delay.h>

#include "matrix.h"
#include "audio/audioproc.h"
#include "remote.h"

#define STBY_ON						1
#define STBY_OFF					0

static uint8_t dispMode = MODE_SND_VOLUME;

static void powerOn(void)
{
	sndPowerOn();
	dispMode = MODE_SND_VOLUME;
	setDisplayTime(TIMEOUT_AUDIO);
}

static void hwInit(void)
{
	rcInit();
	sndInit();
	matrixInit();
	sei();

	_delay_ms(100);
	sndInit();
	powerOn();
}

int main(void)
{
	hwInit();

	int8_t encCnt = 0;
	uint8_t cmd = CMD_END;
	uint8_t input;
	static uint8_t dispPrev = MODE_STANDBY;

	while(1) {
		encCnt = getEncoder();
		cmd = getCmdBuf();

		/* Don't handle buttons in learn mode except some */
		if (dispMode == MODE_LEARN) {
			if (encCnt || cmd != CMD_END)
				setDisplayTime(TIMEOUT_LEARN);
			if (cmd != CMD_BTN_1_LONG && cmd != CMD_BTN_3 && cmd != CMD_BTN_1_2_3_LONG)
				cmd = CMD_END;
		}

		/* Handle command */
		switch (cmd) {
		case CMD_RC_SAVE:
		case CMD_BTN_1:
			sndSave();
			dispPrev = MODE_STANDBY;
			dispMode = MODE_SND_VOLUME;
			setDisplayTime(TIMEOUT_AUDIO);
			break;
		case CMD_RC_MUTE:
		case CMD_BTN_2:
			if (sndGetMute() == MUTE_OFF) {
				sndSetMute(MUTE_ON);
				dispMode = MODE_MUTE;
			} else {
				sndSetMute(MUTE_OFF);
				dispMode = MODE_SND_VOLUME;
			}
			setDisplayTime(TIMEOUT_AUDIO);
			break;
		case CMD_RC_MENU:
		case CMD_BTN_3:
			if (dispMode == MODE_LEARN) {
				nextRcCmd();
				setDisplayTime(TIMEOUT_LEARN);
			} else {
				sndNextParam(&dispMode);
				setDisplayTime(TIMEOUT_AUDIO);
			}
			break;
		case CMD_BTN_1_LONG:
			if (dispMode == MODE_LEARN)
				dispMode = MODE_SND_VOLUME;
			setDisplayTime(TIMEOUT_AUDIO);
			break;
		case CMD_RC_NEXT:
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
		case CMD_BTN_1_2_LONG:
			if (dispMode == MODE_STANDBY)
				dispMode = MODE_LEARN;
			switchTestMode(CMD_RC_SAVE);
			setDisplayTime(TIMEOUT_LEARN);
			break;
		case CMD_BTN_1_2_3_LONG:
			dispMode = MODE_SND_VOLUME;
			setDisplayTime(TIMEOUT_AUDIO);
			resetCodes();
			hwInit();
			dispPrev = MODE_STANDBY;
			dispMode = MODE_SND_VOLUME;
			setDisplayTime(TIMEOUT_AUDIO);

			break;
		}

		/* Emulate RC VOL_UP/VOL_DOWN as encoder actions */
		if (cmd == CMD_RC_VOL_UP)
			encCnt++;
		if (cmd == CMD_RC_VOL_DOWN)
			encCnt--;

		/* Handle encoder */
		if (encCnt) {
			switch (dispMode) {
			case MODE_LEARN:
				break;
			case MODE_STANDBY:
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
		if (getDisplayTime() == 0) {
			if (dispMode == MODE_LEARN) {
				dispMode = MODE_SND_VOLUME;
				setDisplayTime(TIMEOUT_AUDIO);
			} else {
				if (sndGetMute() == MUTE_ON)
					dispMode = MODE_MUTE;
				else
					dispMode = MODE_STANDBY;
			}
		}

		/* Show things */
		switch (dispMode) {
		case MODE_STANDBY:
			showStby();
			break;
		case MODE_MUTE:
			showMute();
			break;
		case MODE_LOUDNESS:
			showLoudness();
			break;
		case MODE_LEARN:
			showLearn();
			break;
		default:
			showSndParam(dispMode, ICON_NATIVE);
			break;
		}

		if (dispMode == dispPrev)
			updateScreen(EFFECT_NONE);
		else
			updateScreen(EFFECT_SPLASH);

		dispPrev = dispMode;
	}

	return 0;
}
