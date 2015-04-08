#ifndef VOLUME_H
#define VOLUME_H

#include "pins.h"
#include "tuner/tuner.h"

#define VOL_MIN				RDA5807_VOL_MIN
#define VOL_MAX				RDA5807_VOL_MAX

void volumeInit(void);

void setVolume(int8_t value);
int8_t getVolume(void);

void muteVolume(void);
void unmuteVolume(void);

void volumeSaveParams(void);

#endif /* VOLUME_H */
