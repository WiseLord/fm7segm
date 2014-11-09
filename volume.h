#ifndef VOLUME_H
#define VOLUME_H

#include "pins.h"

#define VOL_MIN		0
#define VOL_MAX		20

void volumeInit(void);

void setVolume(int8_t value);
int8_t getVolume(void);

void changeVolume(int8_t diff);

#endif /* VOLUME_H */
