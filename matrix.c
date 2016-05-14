#include "matrix.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "remote.h"
#include "eeprom.h"
#include "icons.h"

static uint8_t pos;									/* Current position in framebuffer */

static uint8_t screen[ROWS];						/* Screen buffer */

static volatile uint8_t cmdBuf;
static volatile int8_t encCnt;
static volatile uint16_t displayTime;

static uint8_t rcType;
static uint8_t rcAddr;
static uint8_t rcCode[CMD_RC_END];					/* Array with RC commands */

const static uint8_t font_dig_3x5[] PROGMEM = {
	0x1F, 0x11, 0x1F, // 0
	0x12, 0x1F, 0x10, // 1
	0x1D, 0x15, 0x17, // 2
	0x15, 0x15, 0x1F, // 3
	0x07, 0x04, 0x1F, // 4
	0x17, 0x15, 0x1D, // 5
	0x1F, 0x15, 0x1D, // 6
	0x01, 0x01, 0x1F, // 7
	0x1F, 0x15, 0x1F, // 8
	0x17, 0x15, 0x1F, // 9
	0x00, 0x04, 0x04, // minus
	0x00, 0x00, 0x00, // space
};

const static uint8_t volumeIcon[] PROGMEM = {
	0x10, 0x18, 0x1C, 0x1E, 0x1F
};

const static uint8_t balanceIcon[] PROGMEM = {
	0x1F, 0x0E, 0x04, 0x0E, 0x1F
};

const static uint8_t frontIcon[] PROGMEM = {
	0x11, 0x1B, 0x1F, 0x1B, 0x11
};

const static uint8_t centerIcon[] PROGMEM = {
	0x04, 0x07, 0x07, 0x07, 0x04
};

const static uint8_t subwooferIcon[] PROGMEM = {
	0x0C, 0x0F, 0x0F, 0x0F, 0x0C
};

const static uint8_t muteIcon[] PROGMEM = {
	0x11, 0x0A, 0x04, 0x0A, 0x11
};

static const avrPort ports[ROWS] PROGMEM = {
	{&PORT(ROW_01), ROW_01_LINE},
	{&PORT(ROW_02), ROW_02_LINE},
	{&PORT(ROW_03), ROW_03_LINE},
	{&PORT(ROW_04), ROW_04_LINE},
	{&PORT(ROW_05), ROW_05_LINE},
	{&PORT(ROW_06), ROW_06_LINE},
	{&PORT(ROW_07), ROW_07_LINE},
	{&PORT(ROW_08), ROW_08_LINE},
	{&PORT(ROW_09), ROW_09_LINE},
	{&PORT(ROW_10), ROW_10_LINE},
	{&PORT(ROW_11), ROW_11_LINE},
	{&PORT(ROW_12), ROW_12_LINE},
	{&PORT(ROW_13), ROW_13_LINE},
	{&PORT(ROW_14), ROW_14_LINE},
	{&PORT(ROW_15), ROW_15_LINE},
	{&PORT(ROW_16), ROW_16_LINE},
};


static void matrixShowDig(uint8_t dig)				/* Show decimal digit */
{
	uint8_t i;

	for (i = 0; i < 3; i++) {
		if (pos < ROWS) {
			screen[pos] &= 0xE0;
			screen[pos] |= pgm_read_byte(font_dig_3x5 + dig * 3 + i);
			pos++;
		}
	}
	if (pos < ROWS)
		screen[pos++] &= 0xE0;

	return;
}


static void matrixShowNumber(int8_t value)			/* Show 3-digits decimal number */
{
	uint8_t neg = 0;
	pos = 5;

	if (value < 0) {
		neg = 1;
		value = -value;
	}
	if (value / 10) {
		if (neg)
			matrixShowDig(10); // minus
		else
			matrixShowDig(11); // space
		matrixShowDig(value / 10);
	} else {
		matrixShowDig(11); // space
		if (neg)
			matrixShowDig(10); // minus
		else
			matrixShowDig(11); // space
	}
	matrixShowDig(value % 10);

	return;
}

static void matrixShowBar(int8_t value)				/* Show asimmetric bar 0..16 */
{
	uint8_t i;

	for (i = 0; i < 16; i++) {
		if (value > i)
			screen[i] |= 0xC0;
		else
			screen[i] &= ~0xC0;
	}

	return;
}

static void matrixShowSymBar(int8_t value)
{
	uint8_t i;

	for (i = 0; i < 16; i++) {
		if ((value && ((i < 8 && value < i - 7) || (i >= 8 && value > i - 8))) || (!value && (i == 7 || i == 8)))
			screen[i] |= 0xC0;
		else
			screen[i] &= ~0xC0;
	}


	return;
}

static CmdID rcCmdIndex(uint8_t rcCmd)
{
	CmdID i;

	for (i = 0; i < CMD_RC_END; i++)
		if (rcCmd == rcCode[i])
			return i;

	return CMD_RC_END;
}

ISR (TIMER0_OVF_vect)
{
	// 8000000/256/8 = 3906 polls/sec

	uint8_t i;
	static uint16_t rcTimer;

	static volatile uint8_t stateBtnEnc;			/* Buttons and encoder raw state */

	uint8_t btnNow;
	static uint8_t btnPrev = BTN_STATE_0;
	static int16_t btnCnt = 0;						/* Buttons press duration value */

	uint8_t encNow;
	static uint8_t encPrev = ENC_0;

	static uint8_t row;								/* Current row being scanned */

	row <<= 1;
	if (!row)
		row = 0x01;

	if (row & 0x80)
		PORT(REG_DATA) |= REG_DATA_LINE;
	else
		PORT(REG_DATA) &= ~REG_DATA_LINE;

	for (i = 0; i < ROWS; i++)
		*((uint8_t*)pgm_read_word(&ports[i].port)) &= ~pgm_read_byte(&ports[i].line);

	// Strob 250ns on F_CPU 8MHz
	PORT(REG_CLK) |= REG_CLK_LINE;
	PORT(REG_CLK) &= ~REG_CLK_LINE;

	for (i = 0; i < ROWS; i++)
		if (screen[i] & row)
			*((uint8_t*)pgm_read_word(&ports[i].port)) |= pgm_read_byte(&ports[i].line);

	/* Update buttons state */
	if (PIN(BUTTON) & BUTTON_LINE)
		stateBtnEnc &= ~row;
	else
		stateBtnEnc |= row;

	btnNow = stateBtnEnc & BTN_ALL;
	encNow = stateBtnEnc & ENC_AB;

	/* If encoder event has happened, inc/dec encoder counter */
	switch (encNow) {
	case ENC_AB:
		if (encPrev == ENC_B)
			encCnt++;
		if (encPrev == ENC_A)
			encCnt--;
		break;
/*	case ENC_A:
		if (encPrev == ENC_AB)
			encCnt++;
		if (encPrev == ENC_0)
			encCnt--;
		break;
	case ENC_B:
		if (encPrev == ENC_0)
			encCnt++;
		if (encPrev == ENC_AB)
			encCnt++;
		break;
	case ENC_0:
		if (encPrev == ENC_A)
			encCnt++;
		if (encPrev == ENC_B)
			encCnt++;
		break;
*/	}
	encPrev = encNow;				/* Save current encoder state */

	/* If button event has happened, place it to command buffer */
	if (btnNow) {
		if (btnNow == btnPrev) {
			btnCnt++;
			if (btnCnt == LONG_PRESS) {
				switch (btnPrev) {
				case BTN_1:
					cmdBuf = CMD_BTN_1_LONG;
					break;
				case BTN_2:
					cmdBuf = CMD_BTN_2_LONG;
					break;
				case BTN_3:
					cmdBuf = CMD_BTN_3_LONG;
					break;
				}
			}
		} else {
			btnPrev = btnNow;
		}
	} else {
		if ((btnCnt > SHORT_PRESS) && (btnCnt < LONG_PRESS)) {
			switch (btnPrev) {
			case BTN_1:
				cmdBuf = CMD_BTN_1;
				break;
			case BTN_2:
				cmdBuf = CMD_BTN_2;
				break;
			case BTN_3:
				cmdBuf = CMD_BTN_3;
				break;
			}
		}
		btnCnt = 0;
	}

	/* Place RC5 event to command buffer if enough RC5 timer ticks */
	IRData ir = takeIrData();

	CmdID rcCmdBuf = CMD_END;

	if (ir.ready && (ir.type == rcType && ir.address == rcAddr)) {
		if (!ir.repeat || (rcTimer > RC_LONG_PRESS)) {
			rcTimer = 0;
			rcCmdBuf = rcCmdIndex(ir.command);
		}
		if (ir.command == rcCode[CMD_RC_VOL_UP] || ir.command == rcCode[CMD_RC_VOL_DOWN]) {
			if (rcTimer > RC_VOL_REPEAT) {
				rcTimer = RC_VOL_DELAY;
				rcCmdBuf = rcCmdIndex(ir.command);
			}
		}
	}

	if (cmdBuf == CMD_END)
		cmdBuf = rcCmdBuf;

	/* Timer of current display mode */
	if (displayTime)
		displayTime--;

	/* Time from last IR command */
	if (rcTimer < RC_PRESS_LIMIT)
		rcTimer++;

	return;
}

static void showIcon(const uint8_t iconNum)
{
	uint8_t i;
	uint8_t pgmData;

	const uint8_t *icon;

	icon = &icons[5 * iconNum];

	if (icon) {
		for (i = 0; i < 5; i++) {
			pgmData = pgm_read_byte(icon + i);
			pgmData &= 0x01F;
			screen[i] &= 0xE0;
			screen[i] |= pgmData;
		}
	}

	return;
}

void matrixInit(void)
{
	DDR(ROW_01) |= ROW_01_LINE;
	DDR(ROW_02) |= ROW_02_LINE;
	DDR(ROW_03) |= ROW_03_LINE;
	DDR(ROW_04) |= ROW_04_LINE;
	DDR(ROW_05) |= ROW_05_LINE;
	DDR(ROW_06) |= ROW_06_LINE;
	DDR(ROW_07) |= ROW_07_LINE;
	DDR(ROW_08) |= ROW_08_LINE;
	DDR(ROW_09) |= ROW_09_LINE;
	DDR(ROW_10) |= ROW_10_LINE;
	DDR(ROW_11) |= ROW_11_LINE;
	DDR(ROW_12) |= ROW_12_LINE;
	DDR(ROW_13) |= ROW_13_LINE;
	DDR(ROW_14) |= ROW_14_LINE;
	DDR(ROW_15) |= ROW_15_LINE;
	DDR(ROW_16) |= ROW_16_LINE;

	DDR(REG_DATA) |= REG_DATA_LINE;
	DDR(REG_CLK) |= REG_CLK_LINE;

	TIMSK |= (1<<TOIE0);							/* Enable timer overflow interrupt */
	TCCR0 |= (0<<CS02) | (1<<CS01) | (0 <<CS00);	/* Set timer prescaller to 8 */

	cmdBuf = CMD_END;
	encCnt = 0;

	rcType = eeprom_read_byte((uint8_t*)EEPROM_RC_TYPE);
	rcAddr = eeprom_read_byte((uint8_t*)EEPROM_RC_ADDR);
	eeprom_read_block(rcCode, (uint8_t*)EEPROM_RC_CMD, CMD_RC_END);

	return;
}

void matrixFill(uint8_t data)
{
	uint8_t i;

	for (i = 0; i < ROWS; i++)
		screen[i] = data;

	return;
}

void matrixFadeOff(void)
{
	uint8_t i;

	for (i = 0; i < 8; i++) {
		_delay_ms(10);
		screen[i] = 0;
		screen[15 - i] = 0;
	}

	return;
}

void showSndParam(sndMode mode, uint8_t icon)
{
	sndParam *param = sndParAddr(mode);
	int16_t value = param->value;
	int8_t min = pgm_read_byte(&param->grid->min);
	int8_t max = pgm_read_byte(&param->grid->max);

	if (icon == ICON_NATIVE)
		showIcon(param->icon);
	matrixShowNumber(param->value * ((pgm_read_byte(&param->grid->step) + 4) >> 3));

	if (min + max) {
		max -= min;
		value -= min;
		value *= 17;
		value /= max;
		matrixShowBar(value);
	} else {
		value *= 9;
		value /= max;
		matrixShowSymBar(value);
	}

	return;
}

void showMute(void)
{
	showSndParam(MODE_SND_VOLUME, ICON_OTHER);
	showIcon(ICON_MUTE);

	return;
}

void showLoudness(void)
{
	showSndParam(MODE_SND_VOLUME, ICON_OTHER);
	showIcon(ICON_LOUDNESS);

	return;
}

int8_t getEncoder(void)
{
	int8_t ret = encCnt;
	encCnt = 0;
	return ret;
}

CmdID getCmdBuf(void)
{
	CmdID ret;

	ret = cmdBuf;
	cmdBuf = CMD_END;

	return ret;
}

void setDisplayTime(uint16_t value)
{
	displayTime = POLL_FREQ * value;

	return;
}

uint16_t getDisplayTime(void)
{
	return displayTime / POLL_FREQ;
}
