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

// Indicator type
#define EEPROM_IND_TYPE			0x10

/* FM tuner parameters valus */
#define EEPROM_FM_TUNER			0x20
#define EEPROM_FM_CTRL			(EEPROM_FM_TUNER + 0x01)
#define EEPROM_FM_STEP1			(EEPROM_FM_TUNER + 0x02)
#define EEPROM_FM_STEP2			(EEPROM_FM_TUNER + 0x03)
#define EEPROM_FM_FREQ_MIN		(EEPROM_FM_TUNER + 0x04)
#define EEPROM_FM_FREQ_MAX		(EEPROM_FM_TUNER + 0x06)
#define EEPROM_FM_FREQ			(EEPROM_FM_TUNER + 0x08)
#define EEPROM_FM_MONO			(EEPROM_FM_TUNER + 0x0A)
#define EEPROM_FM_RDS			(EEPROM_FM_TUNER + 0x0B)

/* FM stations */
#define EEPROM_FAV_STATIONS		(EEPROM_FM_TUNER + 0x10)
#define EEPROM_STATIONS			(EEPROM_FM_TUNER + 0x24)

#endif // EEPROM_H
