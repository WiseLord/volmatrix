#ifndef MATRIX_H
#define MATRIX_H

#include <inttypes.h>
#include "pins.h"

#define ROWS		16

void matrixInit();
void matrixSetPos(uint8_t value);
void matrixShowDig(uint8_t dig);
void matrixClear(void);
void matrixShowNumber(int8_t value);
void matrixShowVolBar(int8_t value);

#endif /* MATRIX_H */
