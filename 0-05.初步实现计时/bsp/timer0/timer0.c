#include "timer0.h"


#define TIMER_MS 1
#define TIMER_COUNTS (65536UL - (FOSC / 1000))  // 1ms ����
#define TH0_INIT (TIMER_COUNTS >> 8)
#define TL0_INIT (TIMER_COUNTS & 0xFF)

volatile unsigned long systick = 0;

void Timer0_Init(void)
{
    AUXR |= (1 << 7); // ���ö�ʱ��0Ϊ1Tģʽ

    TH0 = TH0_INIT;
    TL0 = TL0_INIT;

    ET0 = 1;  // ������ʱ��0�ж�
    TR0 = 1;  // ������ʱ��0
}

unsigned long getsystick(void)
{
    return systick;
}


void Timer0_ISR(void) interrupt 1
{
    TH0 = TH0_INIT; // ��װ��
    TL0 = TL0_INIT;
    systick++;
}

