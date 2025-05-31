#include "config.h"
#include "timer0.h"
#include "uart.h"
#include "delay.h"

#include "encoder.h"
#include "beep.h"
#include "oled.h"
#include "bmp.h"


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

void main(void)
{

		gpio_init();
//		UartInit(); 
		encoder_init();
		Timer0_Init();
    Buzzer_Pin_Init(); // 配置引脚
    PWM_Init();        // 初始化PWM
		EA = 1; 
		OLED_Init();//初始化OLED
		OLED_ColorTurn(0);//0正常显示，1 反色显示
		OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
//		printf("system ok! \r\n");
		while(1) 
		{		
			if (KEY == 1 && !key_pressed)
			{
				Delay_ms(20);
				if (KEY == 1)
				{
						key_pressed = 1;
						current_mode = (current_mode == SET_MODE) ? COUNTDOWN_MODE : SET_MODE;
				}
			}
			if (KEY == 0) key_pressed = 0;

			if (current_mode == SET_MODE)
			{
					Buzzer_Set(0,0);
					if(encoder_step != 0)
					{
						UpdateTime(encoder_step);
						encoder_step = 0;
					}
			}
			else if (current_mode == COUNTDOWN_MODE)
			{
				if(timer_flage >= 100)
				{
						timer_flage = 0;
						UpdateTime(-1);
						if(time_h == 0 && time_m == 0 && time_s == 0)
						{
//							Beep_Note(1, 80, 200);
							Play_Melody();
						}
				}
			}
			ShowTime();
		}	  
}

