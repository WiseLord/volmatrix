#ifndef MATRIX_H
#define MATRIX_H

#include <inttypes.h>
#include "pins.h"

#include "icons.h"
#include "audio/audio.h"

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

typedef enum {
	CMD_RC_STBY,
	CMD_RC_MUTE,
	CMD_RC_MENU,
	CMD_RC_VOL_UP,
	CMD_RC_VOL_DOWN,
	CMD_RC_RED,
	CMD_RC_GREEN,
	CMD_RC_YELLOW,
	CMD_RC_BLUE,
	CMD_RC_NEXT,

	CMD_RC_END,

	CMD_BTN_1,
	CMD_BTN_2,
	CMD_BTN_3,
	CMD_BTN_1_LONG,
	CMD_BTN_2_LONG,
	CMD_BTN_3_LONG,

	CMD_BTN_1_2_LONG,

	CMD_END
} CmdID;

enum {
	MODE_STANDBY = MODE_SND_END,

	MODE_MUTE,
	MODE_LOUDNESS,

	MODE_LEARN,

	MODE_END
};

enum {
	ICON_NATIVE,
	ICON_OTHER
};

enum {
	EFFECT_NONE,
	EFFECT_SPLASH,

	EFFECT_END
};

enum {
	SYM_SPACE,
	SYM_MINUS,

	SYM_NUMBERS
};

#define TIMEOUT_AUDIO		2000
#define TIMEOUT_STBY		1000
#define TIMEOUT_LEARN		10000

/* Buttons poll frequency, kHz */
#define POLL_FREQ			4

/* Button press durations, time(ms) * POLL_FREQ */
#define SHORT_PRESS			(100 * POLL_FREQ)
#define LONG_PRESS			(600 * POLL_FREQ)

#define RC_LONG_PRESS		(800 * POLL_FREQ)
#define RC_VOL_DELAY		(360 * POLL_FREQ)
#define RC_VOL_REPEAT		(400 * POLL_FREQ)
#define RC_PRESS_LIMIT		(1000 * POLL_FREQ)

void matrixInit(void);
void matrixFill(uint8_t data);

void showSndParam(sndMode mode, uint8_t icon);
void showMute(void);
void showLoudness(void);
void showStby(void);

void showLearn(void);
void nextRcCmd(void);
void switchTestMode(uint8_t index);

void updateScreen(uint8_t effect);

int8_t getEncoder(void);
CmdID getCmdBuf(void);
CmdID getRcBuf(void);

void setDisplayTime(uint16_t value);
uint16_t getDisplayTime(void);

#endif /* MATRIX_H */
