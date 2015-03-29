#ifndef TDA7448_H
#define TDA7448_H

#include <inttypes.h>

/* I2C address */
#define TDA7448_I2C_ADDR			0b10001000

enum {
	TDA7448_REAR_LEFT = 0,
	TDA7448_REAR_RIGHT,
	TDA7448_FRONT_RIGHT,
	TDA7448_CENTER,
	TDA7448_FRONT_LEFT,
	TDA7448_SUBWOOFER
};

enum {
	TDA7448_SND_VOLUME,
	TDA7448_SND_BALANCE,
	TDA7448_SND_FRONT,
	TDA7448_SND_CENTER,
	TDA7448_SND_SUBWOOFER
};

typedef struct {
	int8_t value;
	int8_t min;
	int8_t max;
} tda7448Param;

#define MUTE_ON						1
#define MUTE_OFF					0
#define STBY_ON						1
#define STBY_OFF					0

/* I2C autoincrement flag */
#define TDA7448_AUTO_INC			0x10
/* Mute value */
#define TDA7448_MUTE				0xC0
/* Number of inputs */
#define TDA7448_LINES_COUNT			6

void tda7448SetSpeakers(void);

void tda7448SetMute(uint8_t val);

void tda7448ChangeParam(uint8_t param, int8_t diff);
int8_t tda7448GetParam(uint8_t param);

#endif /* TDA7448_H */
