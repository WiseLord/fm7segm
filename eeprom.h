#ifndef EEPROM_H
#define EEPROM_H

#include <inttypes.h>

// Audio parameters
#define EEPROM_VOLUME			0x00

// Display parameters
#define EEPROM_DISPLAY			0x02
#define EEPROM_BR_WORK			0x04
#define EEPROM_BR_STBY			0x05

// Misc parameters
#define EEPROM_ENC_RES			0x06
#define EEPROM_HOURZERO			0x07

// FM radio parameters
#define EEPROM_FM_FREQ			0x10
#define EEPROM_FM_FREQ_MIN		0x14
#define EEPROM_FM_FREQ_MAX		0x16

#define EEPROM_FM_TUNER			0x1A
#define EEPROM_FM_MONO			0x1C
#define EEPROM_FM_STEP1			0x1D
#define EEPROM_FM_STEP2			0x1E
#define EEPROM_FM_CTRL			0x1F

#define EEPROM_STATIONS			0x20
#define EEPROM_FAV_STATIONS		0x88

#endif // EEPROM_H
