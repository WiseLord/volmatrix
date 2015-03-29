#ifndef MATRIX_H
#define MATRIX_H

#include <inttypes.h>
#include "pins.h"

#define ROWS				16

#define BTN_STATE_0			0
#define BTN_1				(1<<1)
#define BTN_2				(1<<6)
#define BTN_3				(1<<7)
#define BTN_ALL				(BTN_1 | BTN_2 | BTN_3)

#define ENC_0				0
#define ENC_A				(1<<2)
#define ENC_B				(1<<0)
#define ENC_AB				(ENC_A | ENC_B)

enum {
	CMD_RC5_STBY,
	CMD_RC5_MUTE,
	CMD_RC5_MENU,
	CMD_RC5_VOL_UP,
	CMD_RC5_VOL_DOWN,

	CMD_BTN_1,
	CMD_BTN_2,
	CMD_BTN_3,
	CMD_BTN_1_LONG,
	CMD_BTN_2_LONG,
	CMD_BTN_3_LONG,

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

#define TIMEOUT_AUDIO		2000
#define TIMEOUT_STBY		1000

/* Buttons poll frequency, kHz */
#define POLL_FREQ			5

/* Button press durations, time(ms) * POLL_FREQ */
#define SHORT_PRESS			(100 * POLL_FREQ)
#define LONG_PRESS			(600 * POLL_FREQ)

void matrixInit(void);
void matrixClear(void);

void showAudio(uint8_t param);
void showMute(void);

int8_t getEncoder(void);
uint8_t getCmdBuf(void);

void setDisplayTime(uint16_t value);
uint16_t getDisplayTime(void);

#endif /* MATRIX_H */
