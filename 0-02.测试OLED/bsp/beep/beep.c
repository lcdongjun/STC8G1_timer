#include "beep.h"
#include "uart.h"

// 设置 P3.1 为推挽输出 + 映射 CCP0 到 P3.1
void Buzzer_Pin_Init(void)
{
    P3M1 &= ~(1 << 1);
    P3M0 |=  (1 << 1);     // P3.1 推挽输出
    P_SW1 |= 0x10;         // 映射 CCP0 -> CCP0_2 (P3.1)
}

// 初始化 PWM 输出：PCA + PWM 模块0（CCP0）
void PWM_Init(void)
{
    CCON = 0x00;           // PCA 控制清空
    CMOD = 0x00;           // PCA时钟 = 系统时钟 / 128
    CL = 0x00;             // 清空 PCA 计数器
    CH = 0x00;

    CCAPM0 = 0x42;         // 模块0: PWM 模式 + 比较使能
    PCA_PWM0 = 0x00;       // 8位PWM
    CR = 1;                // 启动 PCA
}

// 设置蜂鸣器输出频率和音量
// freq: 频率（Hz），volume: 占空比（1~99，百分比）
void Buzzer_Set(uint16_t freq, uint8_t volume)
{
    uint32_t pwm_clock = FOSC / 128; // PCA时钟 = 系统时钟/128
    uint32_t period;
    uint8_t duty;

    if(freq == 0 || volume == 0) {
        CCAP0H = 0xFF;  // 占空比 0%，无声
        CCAP0L = 0xFF;
        return;
    }

    period = pwm_clock / freq;
    if(period > 256) period = 256;   // 限制最大周期
    if(period < 10)  period = 10;    // 防止频率太高

    duty = period * (100 - volume) / 100;

    CCAP0H = duty;
    CCAP0L = duty;
}

// 播放单个音符
void Beep_Note(uint16_t freq, uint8_t volume, uint16_t duration_ms)
{
    Buzzer_Set(freq, volume);
    Delay_ms(duration_ms);
    Buzzer_Set(0, 0); // 关闭
    Delay_ms(10);      // 音符间隔
}

// 简单播放一段音乐（多音符）
void Play_Melody(void)
{
    // 简单 C调（do re mi so）
    Beep_Note(523, 90, 300);  // Do
    Beep_Note(587, 90, 300);  // Re
    Beep_Note(659, 90, 300);  // Mi
    Beep_Note(784, 90, 300);  // So
    Beep_Note(659, 90, 300);  // Mi
    Beep_Note(784, 90, 600);  // So
		Delay_ms(500);
}

