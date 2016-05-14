#ifndef REMOTE_H
#define REMOTE_H

#include <inttypes.h>
#include "pins.h"

#define RC5_SHORT_MIN				622		// 889 us ± 30%
#define RC5_SHORT_MAX				1156
#define RC5_LONG_MIN				1245	// 1778 us  ± 30%
#define RC5_LONG_MAX				2773

#define RC5_STBT_MASK				0x3000
#define RC5_TOGB_MASK				0x0800
#define RC5_ADDR_MASK				0x07C0
#define RC5_COMM_MASK				0x003F

typedef enum {
	EVENT_RC5_SHORT_SPACE = 0,
	EVENT_RC5_SHORT_PULSE = 2,
	EVENT_RC5_LONG_SPACE  = 4,
	EVENT_RC5_LONG_PULSE  = 6,
} RC5Event;

typedef enum {
	STATE_RC5_START1 = 0,
	STATE_RC5_MID1   = 1,
	STATE_RC5_MID0   = 2,
	STATE_RC5_START0 = 3,
} RC5State;

#define NEC_PULSE_WIDTH_MIN			392		//  560 us ± 30%
#define NEC_PULSE_WIDTH_MAX			728
#define NEC_ZERO_WIDTH_MIN			392		//  560 us ± 30%
#define NEC_ZERO_WIDTH_MAX			728
#define NEC_ONE_WIDTH_MIN			1176	// 1680 us ± 30%
#define NEC_ONE_WIDTH_MAX			2184
#define NEC_START_PULSE_WIDTH_MIN	6300	// 9000 us ± 30%
#define NEC_START_PULSE_WIDTH_MAX	11700
#define NEC_START_PAUSE_WIDTH_MIN	3150	// 4500 us ± 30%
#define NEC_START_PAUSE_WIDTH_MAX	5850
#define NEC_REPEAT_WIDTH_MIN		1575	// 2250 us ± 30%
#define NEC_REPEAT_WIDTH_MAX		2925

enum {
	IR_TYPE_RC5,
	IR_TYPE_NEC,

	IR_TYPE_NONE = 0x0F
};

typedef struct {
	uint8_t ready : 1;
	uint8_t repeat : 1;
	uint8_t type : 6;
	uint8_t address;
	uint8_t command;
} IRData;

void rcInit(void);

IRData takeIrData(void);
IRData getIrData(void);
void setIrData(uint8_t type, uint8_t addr, uint8_t cmd);

#endif /* REMOTE_H */
