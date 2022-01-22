#ifndef PINS_H
#define PINS_H

#include <avr/io.h>

#define CONCAT(x,y)         x ## y

#define DDR(x)              CONCAT(DDR,x)
#define PORT(x)             CONCAT(PORT,x)
#define PIN(x)              CONCAT(PIN,x)

/* Matrix ports */

#define ROW_16_LINE     (1<<3)
#define ROW_15_LINE     (1<<1)
#define ROW_14_LINE     (1<<0)
#define ROW_13_LINE     (1<<2)
#define ROW_12_LINE     (1<<0)
#define ROW_11_LINE     (1<<1)
#define ROW_10_LINE     (1<<5)
#define ROW_09_LINE     (1<<3)

#define ROW_08_LINE     (1<<7)
#define ROW_07_LINE     (1<<4)
#define ROW_06_LINE     (1<<4)
#define ROW_05_LINE     (1<<2)
#define ROW_04_LINE     (1<<6)
#define ROW_03_LINE     (1<<6)
#define ROW_02_LINE     (1<<5)
#define ROW_01_LINE     (1<<7)

#define ROW_08_06_04_02                     D
#define ROW_08_06_04_02_LINE                (ROW_08_LINE | ROW_06_LINE | ROW_04_LINE | ROW_02_LINE)
#define ROW_16_15_13_12                     C
#define ROW_16_15_13_12_LINE                (ROW_16_LINE | ROW_15_LINE | ROW_13_LINE | ROW_12_LINE)
#define ROW_14_11_10_09_07_05_03_01         B
#define ROW_14_11_10_09_07_05_03_01_LINE    (ROW_14_LINE | ROW_11_LINE | ROW_10_LINE | ROW_09_LINE | ROW_07_LINE | ROW_05_LINE | ROW_03_LINE | ROW_01_LINE)

#define REG_DATA            D
#define REG_DATA_LINE   (1<<1)
#define REG_CLK             D
#define REG_CLK_LINE    (1<<0)

#define BUTTON              D
#define BUTTON_LINE     (1<<2)

#define RC                  D
#define RC_LINE         (1<<3)

#endif /* PINS_H */
