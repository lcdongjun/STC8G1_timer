#ifndef __BEEP_H
#define __BEEP_H

#include "config.h"

void Buzzer_Pin_Init(void);
void PWM_Init(void);
void Buzzer_Set(uint16_t freq, uint8_t volume);
void Beep_Note(uint16_t freq, uint8_t volume, uint16_t duration_ms);
void Play_Melody(void);

#endif

