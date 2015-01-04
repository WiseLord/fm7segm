#ifndef VOLUME_H
#define VOLUME_H

#include "pins.h"

#define VOL_MIN				RDA5807_VOL_MIN
#define VOL_MAX				RDA5807_VOL_MAX

#define eepromVolume		((uint8_t*)0x00)

void volumeInit(void);

void setVolume(int8_t value);
int8_t getVolume(void);

void changeVolume(int8_t diff);

void volumeLoadParams(void);
void volumeSaveParams(void);

void muteVolume(void);
void unmuteVolume(void);

#endif /* VOLUME_H */
