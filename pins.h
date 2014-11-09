#ifndef PINS_H
#define PINS_H

#include <avr/io.h>

#define CONCAT(x,y)			x ## y

#define DDR(x)				CONCAT(DDR,x)
#define PORT(x)				CONCAT(PORT,x)
#define PIN(x)				CONCAT(PIN,x)

#define SEG_A				D
#define SEG_A_LINE			(1<<5)
#define SEG_B				D
#define SEG_B_LINE			(1<<6)
#define SEG_C				B
#define SEG_C_LINE			(1<<1)
#define SEG_D				D
#define SEG_D_LINE			(1<<7)
#define SEG_E				B
#define SEG_E_LINE			(1<<2)
#define SEG_F				B
#define SEG_F_LINE			(1<<7)
#define SEG_G				B
#define SEG_G_LINE			(1<<6)
#define SEG_P				B
#define SEG_P_LINE			(1<<0)

#define DIG_0				C
#define DIG_0_LINE			(1<<0)
#define DIG_1				C
#define DIG_1_LINE			(1<<1)
#define DIG_3				C
#define DIG_3_LINE			(1<<2)
#define DIG_2				C
#define DIG_2_LINE			(1<<3)

#define VOLUME				D
#define VOLUME_LINE			(1<<0)

/* Encoder definitions */
#define ENCODER_A			D
#define ENCODER_A_LINE		(1<<2)
#define ENC_GND				D
#define ENC_GND_LINE		(1<<3)
#define ENCODER_B			D
#define ENCODER_B_LINE		(1<<4)

#endif /* PINS_H */
