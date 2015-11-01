#ifndef EEPROM_H
#define EEPROM_H

/* Audio parameters values */
#define EEPROM_VOLUME				0x00

/* Audio processor values */
#define EEPROM_AUDIOPROC			0x18
#define EEPROM_INPUT				0x19
#define EEPROM_LOUDNESS				0x1A

#define EEPROM_INPUT_ICONS			0x1C

/* RC5 address and command */
#define eepromRC5Addr			((void*)0x14)
#define eepromRC5Cmd			((uint8_t*)0x20)

#endif /* EEPROM_H */

