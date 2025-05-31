#include "timer0.h"
#include "encoder.h"

#define TIMER_MS 10
#define TIMER_COUNTS (65536UL - ((FOSC / 12) * TIMER_MS / 1000))
#define TH0_INIT (TIMER_COUNTS >> 8)
#define TL0_INIT (TIMER_COUNTS & 0xFF)

volatile int timer_flage = 0;

void Timer0_Init(void)
{
    TMOD &= 0xF0;    // ��ʱ��0ģʽ1 (16λ)
    TMOD |= 0x01;

    TH0 = TH0_INIT;
    TL0 = TL0_INIT;

    ET0 = 1;  // ������ʱ��0�ж�
    TR0 = 1;  // ������ʱ��0
}


void Timer0_ISR(void) interrupt 1
{
    TH0 = TH0_INIT;
    TL0 = TL0_INIT;
		if(current_mode == COUNTDOWN_MODE)
		{
				timer_flage++;
		}
		else
		{
			timer_flage = 0;
		}
}

