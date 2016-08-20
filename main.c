#include <avr/interrupt.h>
#include <util/delay.h>

#include "display.h"
#include "audio/audio.h"
#include "remote.h"

#define STBY_ON						1
#define STBY_OFF					0

static uint8_t dispMode = MODE_STANDBY;

static void powerOn(void)
{
	sndPowerOn();
	dispMode = MODE_SND_VOLUME;
	setDisplayTime(TIMEOUT_AUDIO);
}

static void powerOff(void)
{
	sndSetMute(1);
	sndPowerOff();
	dispMode = MODE_STANDBY;
	setDisplayTime(TIMEOUT_STBY);
}

int main(void)
{
	rcInit();
	sndInit();
	matrixInit();
	sei();

	_delay_ms(100);
	powerOff();
	sndInit();

	int8_t encCnt = 0;
	uint8_t cmd = CMD_END;
	static uint8_t dispPrev = MODE_STANDBY;

	while(1) {
		encCnt = getEncoder();
		cmd = getCmdBuf();

		/* If no command from buttons, try remote control */
		if (cmd == CMD_END)
			cmd = getRcBuf();

		/* Don't handle commands in standby mode except some */
		if (dispMode == MODE_STANDBY) {
			encCnt = 0;
			if (cmd != CMD_RC_STBY && cmd != CMD_BTN_1 && cmd != CMD_BTN_1_2_LONG)
				cmd = CMD_END;
		}
		/* Don't handle buttons in learn mode except some */
		if (dispMode == MODE_LEARN) {
			if (encCnt || cmd != CMD_END)
				setDisplayTime(TIMEOUT_LEARN);
			if (cmd != CMD_BTN_1_LONG && cmd != CMD_BTN_3)
				cmd = CMD_END;
		}

		/* Handle command */
		switch (cmd) {
		case CMD_RC_STBY:
		case CMD_BTN_1:
			if (dispMode == MODE_STANDBY)
				powerOn();
			else
				powerOff();
			break;
		case CMD_RC_MUTE:
		case CMD_BTN_2:
			if (aproc.mute) {
				sndSetMute(0);
				dispMode = MODE_SND_VOLUME;
			} else {
				sndSetMute(1);
				dispMode = MODE_MUTE;
			}
			setDisplayTime(TIMEOUT_AUDIO);
			break;
		case CMD_RC_MENU:
		case CMD_BTN_3:
			if (dispMode == MODE_LEARN) {
				nextRcCmd();
			} else {
				sndNextParam(&dispMode);
				setDisplayTime(TIMEOUT_AUDIO);
			}
			break;
		case CMD_RC_RED:
		case CMD_RC_GREEN:
		case CMD_RC_YELLOW:
		case CMD_RC_BLUE:
			sndSetInput(cmd - CMD_RC_RED);
			dispMode = MODE_SND_GAIN0 + (cmd - CMD_RC_RED);
			setDisplayTime(TIMEOUT_AUDIO);
			break;
		case CMD_BTN_1_LONG:
			if (dispMode == MODE_LEARN)
				powerOff();
			break;
		case CMD_BTN_2_LONG:
			sndSwitchExtra(APROC_EXTRA_LOUDNESS);
			if (aproc.extra & APROC_EXTRA_LOUDNESS)
				dispMode = MODE_LOUDNESS;
			else
				dispMode = MODE_SND_VOLUME;
			setDisplayTime(TIMEOUT_AUDIO);
			break;
		case CMD_RC_NEXT:
		case CMD_BTN_3_LONG:
			if (dispMode >= MODE_SND_GAIN0 && dispMode < MODE_SND_END)
				aproc.input++;
			sndSetInput(aproc.input);
			dispMode = MODE_SND_GAIN0 + aproc.input;
			setDisplayTime(TIMEOUT_AUDIO);
			break;
		case CMD_BTN_1_2_LONG:
			if (dispMode == MODE_STANDBY)
				dispMode = MODE_LEARN;
			switchTestMode(CMD_RC_STBY);
			setDisplayTime(TIMEOUT_LEARN);
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
			case MODE_STANDBY:
			case MODE_LEARN:
				break;
			case MODE_MUTE:
			case MODE_LOUDNESS:
			case MODE_TIME:
				dispMode = MODE_SND_VOLUME;
			default:
				sndSetMute(0);
				sndChangeParam(dispMode, encCnt);
				setDisplayTime(TIMEOUT_AUDIO);
				break;
			}
		}

		/* Exid to default mode if timer expired */
		if (getDisplayTime() == 0) {
			if (dispMode == MODE_LEARN || dispMode == MODE_STANDBY) {
				dispMode = MODE_STANDBY;
			} else {
				if (aproc.mute)
					dispMode = MODE_MUTE;
				else
					dispMode = MODE_TIME;
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
		case MODE_TIME:
			showTime();
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
