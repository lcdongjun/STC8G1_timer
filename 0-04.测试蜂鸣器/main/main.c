#include "config.h"
#include "timer0.h"
#include "uart.h"
#include "delay.h"

#include "encoder.h"
#include "beep.h"
#include "oled.h"
#include "bmp.h"

volatile bit beep_active = 0;        // 蜂鸣器开关状态
volatile bit beep_on = 0;
unsigned long t0 = 0,time_update = 0;
void Handle_Buzzer(bit on);

void gpio_init(void)
{
		// P30 - 编码器按下按钮输入
		P3M1 |=  (1 << 0);  // 清除 P3.0 的 M1 位
		P3M0 &= ~(1 << 0);  // 清除 P3.0 的 M0 位

		// P31 - 蜂鸣器控制输出
		P3M1 &= ~(1 << 1);  // 清除 P3.1 的 M1 位
		P3M0 |=  (1 << 1);  // 设置 P3.1 的 M0 位 -> 推挽输出

		// P32 - I2C SDA
		P3M1 |=  (1 << 2);  // 设置 P3.2 的 M1 位
		P3M0 |=  (1 << 2);  // 设置 P3.2 的 M0 位 -> 开漏输出（I2C 要求）

		// P33 - I2C SCL
		P3M1 |=  (1 << 3);  // 设置 P3.3 的 M1 位
		P3M0 |=  (1 << 3);  // 设置 P3.3 的 M0 位 -> 开漏输出（I2C 要求）

		// P54 - 编码器 A 相输入
		P5M1 &= ~(1 << 4);  // 清除 P5.4 的 M1 位
		P5M0 &= ~(1 << 4);  // 清除 P5.4 的 M0 位 -> 准双向口（输入）

		// P55 - 编码器 B 相输入
		P5M1 &= ~(1 << 5);  // 清除 P5.5 的 M1 位
		P5M0 &= ~(1 << 5);  // 清除 P5.5 的 M0 位 -> 准双向口（输入）
}
// 按键切换模式并控制蜂鸣器
void Handle_KeyPress(){
    static bit key_last = 0;
    static unsigned long key_down_time = 0;

    if (KEY == 1) {
        if (!key_last) {
            key_last = 1;
            key_down_time = getsystick();  // 记录按下时间
        } else {
            if (getsystick() - key_down_time >= 1200) {  // 长按 1.2 秒
                current_mode = (current_mode == SET_MODE) ? COUNTDOWN_MODE : SET_MODE;
								OLED_Clear();
								PWM_Set(1200, 50);
								PWM_ON();
                Delay_ms(100);
								PWM_OFF();
                ShowTime();
                while (KEY == 1);
                key_last = 0;
								beep_on = 0;
            }
        }
    } else {
        key_last = 0;
    }
}
// 旋转编码器设置时间
void Handle_Encoder() {
		if (sampling_ready)
		{
				int sum = 0;
				char i = 0;
				for (i = 0; i < SAMPLE_COUNT; i++)
						sum += encoder_samples[i];

				if (sum > 0)
						encoder_step = +1;
				else if (sum < 0)
						encoder_step = -1;
				else
						encoder_step = 0; // 干扰忽略

				sampling_ready = 0;
		}	
    if (current_mode == SET_MODE && encoder_step != 0) {
				PWM_Set(1400, 50);
				PWM_ON();
				Delay_ms(50);
				PWM_OFF();
				UpdateTime(encoder_step*10);
				encoder_step = 0;
    }
}

// 倒计时逻辑
void Handle_Countdown() {
    if (current_mode == COUNTDOWN_MODE && getsystick() - time_update >= 1000) {
        time_update = getsystick();
        UpdateTime(-1);
        if (time_h == 0 && time_m == 0 && time_s == 1 && !beep_active) {
            beep_on = 1;
        }
    }
}

// 蜂鸣器控制
void Handle_Buzzer(bit on)
{
    static unsigned long t0 = 0;
    unsigned long elapsed;

    if (on)
    {
        if (!beep_active)
        {
            beep_active = 1;
            t0 = getsystick();  // 初次开启蜂鸣器，记录起始时间
        }

        elapsed = getsystick() - t0;

        if (elapsed < 250)
            PWM_Set(1200, 50);
        else if (elapsed < 500)
            PWM_Set(1000, 50);
        else if (elapsed < 750)
            PWM_Set(800, 50);
        else if (elapsed < 1000)
            PWM_Set(600, 50);
        else if (elapsed < 1250)
            PWM_Set(400, 50);
        else
            t0 = getsystick();  // 重置时间，重新开始一轮节奏

        PWM_ON(); // 确保始终保持开启
    }
    else
    {
        PWM_OFF();
        beep_active = 0;
        t0 = getsystick(); // 也可清零计时，防止脉冲残留
    }
}


void main(void)
{

		gpio_init();
		encoder_init();
		Timer0_Init();
		OLED_Init();//初始化OLED
		OLED_ColorTurn(0);//0正常显示，1 反色显示
		OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
		BEEP = 0;
		PWM_Set(1200, 50);
		EA = 1; 
		time_update = getsystick();
		while(1) 
		{
        Handle_KeyPress();
        Handle_Encoder();
        Handle_Countdown();
				Handle_Buzzer(beep_on);
        ShowTime();
		}
}

