#ifndef EEPROM_H
#define EEPROM_H

#include <inttypes.h>

/* Parameters values */

#define eepromFMFreq	((void*)0x16)
#define eepromFMCtrl	((void*)0x18)
#define eepromFMMono	((void*)0x19)
#define eepromFMStep	((void*)0x1A)

/* FM stations */
#define eepromStations	((uint16_t*)0x70)

#endif /* EEPROM_H */
