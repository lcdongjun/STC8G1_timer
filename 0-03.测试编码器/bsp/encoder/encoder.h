#ifndef __ENCODER_H
#define __ENCODER_H

#include "config.h"

sbit ENCODER_A = P5^4;
sbit ENCODER_B = P5^5;
sbit KEY = P3^0;


enum Mode { SET_MODE, COUNTDOWN_MODE };

extern volatile int encoder_count;
extern volatile int time_h;
extern volatile int time_m;
extern volatile int time_s;
extern volatile char encoder_step;
extern volatile bit key_pressed;

extern enum Mode current_mode;

void encoder_init(void);

#endif

