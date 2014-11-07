#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "segm.h"

#include "pins.h"

void hwInit(void)
{
	segmInit();

	sei();
	return;
}

int main(void)
{
	hwInit();

	segmNum(-1079, 1);
	while (1) {
	}

	return 0;
}

