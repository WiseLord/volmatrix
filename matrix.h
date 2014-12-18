#ifndef MATRIX_H
#define MATRIX_H

#include <inttypes.h>
#include "pins.h"

#define ROWS		16

#define BTN_STATE_0		0
#define BTN_0			(1<<0)
#define BTN_1			(1<<1)
#define BTN_2			(1<<2)
#define BTN_3			(1<<3)
#define BTN_4			(1<<4)
#define BTN_5			(1<<5)

enum {
	CMD_RC5_STBY,
	CMD_RC5_MUTE,
	CMD_RC5_MENU,
	CMD_RC5_VOL_UP,
	CMD_RC5_VOL_DOWN,

	CMD_BTN_0,
	CMD_BTN_1,
	CMD_BTN_2,
	CMD_BTN_3,
	CMD_BTN_4,
	CMD_BTN_5,
	CMD_BTN_0_LONG,
	CMD_BTN_1_LONG,
	CMD_BTN_2_LONG,
	CMD_BTN_3_LONG,
	CMD_BTN_4_LONG,
	CMD_BTN_5_LONG,

	CMD_EMPTY = 0xEF
};

enum {
	MODE_STANDBY,
	MODE_VOLUME,
	MODE_BALANCE,
	MODE_FRONT,
	MODE_CENTER,
	MODE_SUBWOOFER,
	MODE_MUTE
};

// Button press durations */
#define SHORT_PRESS		100
#define LONG_PRESS		600

void matrixInit(void);
void matrixClear(void);

void showVolume(int8_t value);
void showBalance(int8_t value);
void showFront(int8_t value);
void showCenter(int8_t value);
void showSubwoofer(int8_t value);

uint8_t getCmdBuf(void);

void setDisplayTime(uint16_t value);
uint16_t getDisplayTime(void);

#endif /* MATRIX_H */
