#ifndef TDA7448_H
#define TDA7448_H

#include <inttypes.h>
#include "audio.h"

/* I2C address */
#define TDA7448_I2C_ADDR			0b10001000

/* I2C function selection */
enum {
	TDA7448_REAR_LEFT = 0,			/* IN1, pins 2  and 7  */
	TDA7448_REAR_RIGHT,				/* IN2, pins 19 and 14 */
	TDA7448_FRONT_RIGHT,			/* IN3, pins 3  and 6  */
	TDA7448_CENTER,					/* IN4, pins 18 and 15 */
	TDA7448_FRONT_LEFT,				/* IN5, pins 4  and 5  */
	TDA7448_SUBWOOFER,				/* IN6, pins 17 and 16 */
};

#define TDA7448_LINES_COUNT			6

/* I2C autoincrement flag */
#define TDA7448_AUTO_INC			0x10
/* Mute value */
#define TDA7448_MUTE				0xC0
/* Number of inputs */
#define TDA7448_IN_CNT				1
#define TDA7448MIX_IN_CNT			3

void tda7448Init(sndParam *sp);
void tda7448SetSpeakers(int8_t val);

void tda7448SetMute(uint8_t val);

void tda7448MixSetSpeakers(int8_t val);
void tda7448MixSetMute(int8_t val);
void tda7448MixSetInput(uint8_t in);

#endif /* TDA7448_H */
