#include "matrix.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "tda7448.h"

static uint8_t pos;									/* Current position in framebuffer */
static volatile uint8_t row;						/* Current row being scanned */

static uint8_t screen[ROWS];						/* Screen buffer */

static volatile uint8_t cmdBuf;
static volatile uint16_t displayTime;

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

static avrPort ports[ROWS] = {
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

static void matrixshowBalBar(int8_t value)
{
	uint8_t i;

	for (i = 0; i < 16; i++) {
		if ((i < 8 && value < i - 7) || (i >= 8 && value > i - 8))
			screen[i] |= 0xC0;
		else
			screen[i] &= ~0xC0;
	}


	return;
}

ISR (TIMER0_OVF_vect)								/* 8000000 / 64 / (256 - 131) = 1kHz */
{
	uint8_t i;

	uint8_t btnNow;
	static volatile uint8_t btnState;				/* Button state */
	static uint8_t btnPrev = 0;
	static int16_t btnCnt = 0;						/* Buttons press duration value */

	TCNT0 = 131;

	if (++row >= 8)
		row = 0;

	if (row == 7)
		PORT(REG_DATA) |= REG_DATA_LINE;
	else
		PORT(REG_DATA) &= ~REG_DATA_LINE;
	PORT(REG_CLK) |= REG_CLK_LINE;
	asm("nop");
	PORT(REG_CLK) &= ~REG_CLK_LINE;
	for (i = 0; i < ROWS; i++) {
		if (screen[i] & (1<<row))
			*ports[i].port |= ports[i].mask;
		else
			*ports[i].port &= ~ports[i].mask;
	}

	/* Update buttons state */
	if (PIN(BUTTON) & BUTTON_LINE)
		btnState &= ~(1<<row);
	else
		btnState |= (1<<row);

	btnNow = btnState & 0x3F;

	/* If button event has happened, place it to command buffer */
	if (btnNow) {
		if (btnNow == btnPrev) {
			btnCnt++;
			if (btnCnt == LONG_PRESS) {
				switch (btnPrev) {
				case (1<<0):
					cmdBuf = CMD_BTN_0_LONG;
					break;
				case (1<<1):
					cmdBuf = CMD_BTN_1_LONG;
					break;
				case (1<<2):
					cmdBuf = CMD_BTN_2_LONG;
					break;
				case (1<<3):
					cmdBuf = CMD_BTN_3_LONG;
					break;
				case (1<<4):
					cmdBuf = CMD_BTN_4_LONG;
					break;
				case (1<<5):
					cmdBuf = CMD_BTN_5_LONG;
					break;
				}
			}
		} else {
			btnPrev = btnNow;
		}
	} else {
		if ((btnCnt > SHORT_PRESS) && (btnCnt < LONG_PRESS)) {
			switch (btnPrev) {
			case (1<<0):
				cmdBuf = CMD_BTN_0;
				break;
			case (1<<1):
				cmdBuf = CMD_BTN_1;
				break;
			case (1<<2):
				cmdBuf = CMD_BTN_2;
				break;
			case (1<<3):
				cmdBuf = CMD_BTN_3;
				break;
			case (1<<4):
				cmdBuf = CMD_BTN_4;
				break;
			case (1<<5):
				cmdBuf = CMD_BTN_5;
				break;
			}
		}
		btnCnt = 0;
	}

	/* Timer of current display mode */
	if (displayTime)
		displayTime--;

	return;
}

static void showIcon(const uint8_t *icon)
{
	uint8_t i;

	for (i = 0; i < 5; i++) {
		screen[i] &= 0xE0;
		screen[i] |= pgm_read_byte(icon + i);
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
	TCCR0 |= (0<<CS02) | (1<<CS01) | (1 <<CS00);	/* Set timer prescaller to 64 */

	return;
}

void matrixClear(void)
{
	uint8_t i;

	for (i = 0; i < ROWS; i++)
		screen[i] = 0x00;

	return;
}

void showAudio(uint8_t param)
{
	int8_t value = tda7448GetParam(param);

	matrixShowNumber(value);

	switch (param) {
	case TDA7448_SND_VOLUME:
		value += 81;	/* Shift scale (-79..0 => 2..81) */
		value /= 5;		/* Normalize volume value */
		matrixShowBar(value);
		showIcon(volumeIcon);
		break;
	case TDA7448_SND_BALANCE:
		matrixshowBalBar(value);
		showIcon(balanceIcon);
		break;
	case TDA7448_SND_FRONT:
		matrixshowBalBar(value);
		showIcon(frontIcon);
		break;
	case TDA7448_SND_CENTER:
		value += 16;	/* Shift scale (-16..0 => 0..16) */
		matrixShowBar(value);
		showIcon(centerIcon);
		break;
	case TDA7448_SND_SUBWOOFER:
		value += 16;	/* Shift scale (-16..0 => 0..16) */
		matrixShowBar(value);
		showIcon(subwooferIcon);
		break;
	}

	return;
}

uint8_t getCmdBuf(void)
{
	uint8_t ret = cmdBuf;
	cmdBuf = CMD_EMPTY;

	return ret;
}

void setDisplayTime(uint16_t value)
{
	displayTime = value;

	return;
}

uint16_t getDisplayTime(void)
{
	return displayTime;
}
