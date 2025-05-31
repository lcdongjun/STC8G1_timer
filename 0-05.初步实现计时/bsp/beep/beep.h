#ifndef __BEEP_H
#define __BEEP_H

#include "config.h"

sbit BEEP = P3^1;

void PWM_Set(unsigned int freq_hz, unsigned char duty_percent);
void PWM_ON(void);
void PWM_OFF(void);

#endif

