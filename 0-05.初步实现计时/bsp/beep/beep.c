#include "beep.h"
#include "uart.h"


unsigned char  highTicks = 0;
unsigned char  lowTicks = 0;
unsigned char  pwmCounter = 0;
unsigned int  totalTicks = 0;
unsigned int  reload = 0;
bit pwmHigh = 1;

// ========== 核心封装函数：设置PWM频率和占空比 ==========
void PWM_Set(unsigned int freq_hz, unsigned char duty_percent)
{
    static unsigned int last_freq = 0;
    static unsigned char last_duty = 0;
    unsigned long period_us;
    unsigned int pwmInterval_us;

    // 限制 duty 范围（1 ~ 99）
    if (duty_percent < 1) duty_percent = 1;
    if (duty_percent > 99) duty_percent = 99;

    // 如果频率和占空比都没变，直接返回，避免重复设置
    if (freq_hz == last_freq && duty_percent == last_duty)
        return;

    last_freq = freq_hz;
    last_duty = duty_percent;


    // 计算周期：1s = 1,000,000us
    period_us = 1000000UL / freq_hz;
    pwmInterval_us = 50;

    // 总tick数 = 总周期 / 每tick间隔
    totalTicks = period_us / pwmInterval_us;
    highTicks = (totalTicks * duty_percent) / 100;
    lowTicks = totalTicks - highTicks;

    // 设置 Timer1 为 pwmInterval_us（50us）中断周期
    TMOD &= 0x0F;
    TMOD |= 0x10;  // 定时器1，16位

    reload = 65536 - (FOSC / 12 / 1000000 * pwmInterval_us);
    TH1 = reload >> 8;
    TL1 = reload & 0xFF;

    TR1 = 1;
    ET1 = 1;
    EA  = 1;
}
void PWM_OFF(void)
{		BEEP = 0;
    TR1 = 0;
		ET1 = 0;
}
void PWM_ON(void)
{
		BEEP = 1;
    TR1 = 1;
		ET1 = 1;
}
// ========== 定时器1中断服务：生成PWM ==========
void TM1_Isr(void) interrupt 3
{
    // 重载计数值（保持 50us 中断周期）
    reload = 65536 - (FOSC / 12 / 1000000 * 50);
    TH1 = reload >> 8;
    TL1 = reload & 0xFF;

    pwmCounter++;
    if (pwmHigh)
    {
        if (pwmCounter >= highTicks)
        {
            pwmCounter = 0;
            pwmHigh = 0;
            BEEP = 0;
        }
    }
    else
    {
        if (pwmCounter >= lowTicks)
        {
            pwmCounter = 0;
            pwmHigh = 1;
            BEEP = 1;
        }
    }
}


