#include "matrix.h"

#include <avr/io.h>
#include <avr/interrupt.h>

static uint8_t pos;									/* Current position in framebuffer */

static uint8_t screen[ROWS] = {
	0x7E, 0x11, 0x11, 0x11, 0x7E, 0x00, 0x7F, 0x09,
	0x19, 0x29, 0x46, 0x01, 0x01, 0x7F, 0x01, 0x01
};

static uint8_t dig3x5[] = {
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
	0x04, 0x04, 0x04, // minus
	0x00, 0x00, 0x00, // space
};

static uint8_t row;

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

void matrixInit()
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
	TCCR0 |= (0<<CS02) | (1<<CS01) | (1<<CS00);		/* Set timer prescaller to 64 */

	return;
}

ISR (TIMER0_OVF_vect)								/* 8000000 / 64 / (256 - 131) = 1kHz */
{
	TCNT0 = 131;
	uint8_t i;

	if (++row >= 8)
		row = 0;

	if (row == 7)
		PORT(REG_DATA) &= ~REG_DATA_LINE;
	else
		PORT(REG_DATA) |= REG_DATA_LINE;
	PORT(REG_CLK) |= REG_CLK_LINE;
	asm("nop");
	PORT(REG_CLK) &= ~REG_CLK_LINE;
	for (i = 0; i < ROWS; i++) {
		if (screen[i] & (1<<row))
			*ports[i].port |= ports[i].mask;
		else
			*ports[i].port &= ~ports[i].mask;
	}

	return;
}

void matrixSetPos(uint8_t value)
{
	pos = value;

	return;
}

void matrixShowDig(uint8_t dig)
{
	uint8_t i;

	for (i = 0; i < 3; i++) {
		if (pos < ROWS) {
			screen[pos] &= 0xE0;
			screen[pos] |= dig3x5[dig * 3 + i];
			pos++;
		}
	}
	if (pos < ROWS)
		screen[pos++] &= 0xE0;

	return;
}

void matrixClear(void)
{
	uint8_t i;

	for (i = 0; i < ROWS; i++)
		screen[i] = 0x00;

	return;
}

void matrixShowNumber(int8_t value)
{
	uint8_t neg = 0;

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

void matrixShowVolBar(int8_t value)
{
	uint8_t i;

	/* Shift scale (-79..0 => 2..81) and normalize volume value */
	value += 81;
	value /= 5;

	for (i = 0; i < 16; i++) {
		if (value > i)
			screen[i] |= 0xC0;
		else
			screen[i] &= ~0xC0;
	}

	return;
}
