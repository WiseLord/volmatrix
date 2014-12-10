#include "matrix.h"

#include <avr/io.h>
#include <avr/interrupt.h>

static uint8_t screen[ROWS] = {
	0x7E, 0x11, 0x11, 0x11, 0x7E, 0x00, 0x7F, 0x09, 0x19, 0x29, 0x46, 0x01, 0x01, 0x7F, 0x01, 0x01
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
