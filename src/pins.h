#ifndef PINS_H
#define PINS_H

#include <avr/io.h>

#define CONCAT(x,y)         x ## y

#define DDR(x)              CONCAT(DDR,x)
#define PORT(x)             CONCAT(PORT,x)
#define PIN(x)              CONCAT(PIN,x)

/*
 * Pinout selection
 *
 * _PIN1 is a pinout for schematic in article
 * http://radiokot.ru/circuit/digital/home/202/
 *
 * _PIN2 is one for schematic in first pages of forum thread
 * http://radiokot.ru/forum/viewtopic.php?t=109632
 *
 * _PIN3 is one more for schematic and PCB on 4-th page of forum thread
 * http://radiokot.ru/forum/viewtopic.php?p=2217907#p2217907
 *
 */

#if !defined(_PIN1) && !defined(_PIN2) && !defined(_PIN3)
#define _PIN1
#endif

#if defined(_PIN1)

#define SEG_A               B
#define SEG_A_LINE          (1<<6)
#define SEG_B               D
#define SEG_B_LINE          (1<<6)
#define SEG_C               B
#define SEG_C_LINE          (1<<1)
#define SEG_D               C
#define SEG_D_LINE          (1<<1)
#define SEG_E               C
#define SEG_E_LINE          (1<<0)
#define SEG_F               B
#define SEG_F_LINE          (1<<7)
#define SEG_G               B
#define SEG_G_LINE          (1<<0)
#define SEG_P               B
#define SEG_P_LINE          (1<<2)

#define DIG_0               D
#define DIG_0_LINE          (1<<7)
#define DIG_1               D
#define DIG_1_LINE          (1<<5)
#define DIG_2               D
#define DIG_2_LINE          (1<<3)
#define DIG_3               D
#define DIG_3_LINE          (1<<4)

/* Encoder definitions */
#define ENCODER_A           B
#define ENCODER_A_LINE      (1<<4)
#define ENCODER_B           B
#define ENCODER_B_LINE      (1<<3)

/* Buttons definitions */
#define BUTTON_1            C
#define BUTTON_1_LINE       (1<<2)
#define BUTTON_2            D
#define BUTTON_2_LINE       (1<<0)
#define BUTTON_3            D
#define BUTTON_3_LINE       (1<<1)
#define BUTTON_4            D
#define BUTTON_4_LINE       (1<<2)

/* 1-wire definitions */
#define ONE_WIRE            B
#define ONE_WIRE_LINE       (1<<5)

/* Volume PWM line */
#define VOLUME              C
#define VOLUME_LINE         (1<<3)

#elif defined(_PIN2)

#define SEG_A               D
#define SEG_A_LINE          (1<<5)
#define SEG_B               D
#define SEG_B_LINE          (1<<6)
#define SEG_C               B
#define SEG_C_LINE          (1<<1)
#define SEG_D               D
#define SEG_D_LINE          (1<<7)
#define SEG_E               B
#define SEG_E_LINE          (1<<2)
#define SEG_F               B
#define SEG_F_LINE          (1<<7)
#define SEG_G               B
#define SEG_G_LINE          (1<<6)
#define SEG_P               B
#define SEG_P_LINE          (1<<0)

#define DIG_0               C
#define DIG_0_LINE          (1<<0)
#define DIG_1               C
#define DIG_1_LINE          (1<<1)
#define DIG_2               C
#define DIG_2_LINE          (1<<3)
#define DIG_3               C
#define DIG_3_LINE          (1<<2)

/* Encoder definitions */
#define ENCODER_A           B
#define ENCODER_A_LINE      (1<<4)
#define ENCODER_B           B
#define ENCODER_B_LINE      (1<<3)

/* Buttons definitions */
#define BUTTON_1            D
#define BUTTON_1_LINE       (1<<1)
#define BUTTON_2            D
#define BUTTON_2_LINE       (1<<2)
#define BUTTON_3            D
#define BUTTON_3_LINE       (1<<3)
#define BUTTON_4            D
#define BUTTON_4_LINE       (1<<4)

/* 1-wire definitions */
#define ONE_WIRE            B
#define ONE_WIRE_LINE       (1<<5)

/* Volume PWM line */
#define VOLUME              D
#define VOLUME_LINE         (1<<0)

#elif defined(_PIN3)

#define SEG_A               D
#define SEG_A_LINE          (1<<4)
#define SEG_B               D
#define SEG_B_LINE          (1<<6)
#define SEG_C               B
#define SEG_C_LINE          (1<<1)
#define SEG_D               B
#define SEG_D_LINE          (1<<3)
#define SEG_E               B
#define SEG_E_LINE          (1<<2)
#define SEG_F               B
#define SEG_F_LINE          (1<<7)
#define SEG_G               B
#define SEG_G_LINE          (1<<0)
#define SEG_P               B
#define SEG_P_LINE          (1<<4)

#define DIG_0               D
#define DIG_0_LINE          (1<<7)
#define DIG_1               D
#define DIG_1_LINE          (1<<5)
#define DIG_2               B
#define DIG_2_LINE          (1<<6)
#define DIG_3               D
#define DIG_3_LINE          (1<<3)

/* Encoder definitions */
#define ENCODER_A           D
#define ENCODER_A_LINE      (1<<1)
#define ENCODER_B           D
#define ENCODER_B_LINE      (1<<2)

/* Buttons definitions */
#define BUTTON_1            C
#define BUTTON_1_LINE       (1<<0)
#define BUTTON_2            C
#define BUTTON_2_LINE       (1<<1)
#define BUTTON_3            C
#define BUTTON_3_LINE       (1<<2)
#define BUTTON_4            C
#define BUTTON_4_LINE       (1<<3)

/* 1-wire definitions */
#define ONE_WIRE            B
#define ONE_WIRE_LINE       (1<<5)

/* Volume PWM line */
#define VOLUME              D
#define VOLUME_LINE         (1<<0)

#endif

// LM7001 is not supported, but requires pin description

#define LM7001_DATA         VOLUME
#define LM7001_DATA_LINE    VOLUME_LINE
#define LM7001_CL           VOLUME
#define LM7001_CL_LINE      VOLUME_LINE
#define LM7001_CE           VOLUME
#define LM7001_CE_LINE      VOLUME_LINE

#endif /* PINS_H */
