#ifndef TDA7448_H
#define TDA7448_H

#include <inttypes.h>

enum {
	TDA7448_SP_FRONTLEFT,
	TDA7448_SP_FRONTRIGHT,
	TDA7448_SP_REARLEFT,
	TDA7448_SP_REARRIGHT,
	TDA7448_SP_CENTER,
	TDA7448_SP_SUBWOOFER
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

#define TDA7448_CHANNELS	6

#define TDA7448_SP_MIN		-79
#define TDA7448_SP_MAX		0

#define TDA7448_BAL_MIN		-8
#define TDA7448_BAL_MAX		8

#define TDA7448_FRONT_MIN	-8
#define TDA7448_FRONT_MAX	8

#define TDA7448_CENTER_MIN	-16
#define TDA7448_CENTER_MAX	0

#define TDA7448_SUB_MIN		-16
#define TDA7448_SUB_MAX		0

void tda7448SetSpeakers(void);

void tda7448IncParam(uint8_t param);
void tda7448DecParam(uint8_t param);
int8_t tda7448GetParam(uint8_t param);

#endif /* TDA7448_H */
