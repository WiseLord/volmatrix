#ifndef EEPROM_H
#define EEPROM_H

/* Audio parameters values */
#define EEPROM_VOLUME               0x00

/* Audio parameters values */
#define EEPROM_AUDIOPROC            0x20
#define EEPROM_MAX_INPUT_CNT        0x21
#define EEPROM_INPUT                0x22
#define EEPROM_APROC_EXTRA          0x23

#define EEPROM_INPUT_ICONS          0x28

/* Display parameters */
#define EEPROM_BR_STBY              0x32
#define EEPROM_BR_WORK              0x33

/* Other parameters */
#define EEPROM_ENC_RES              0x36

/* RC eeprom data*/
#define EEPROM_RC_TYPE              0x38
#define EEPROM_RC_ADDR              0x39
#define EEPROM_RC_CMD               0x40

#endif /* EEPROM_H */

