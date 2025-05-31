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
		// P30 - ���������°�ť����
		P3M1 |=  (1 << 0);  // ��� P3.0 �� M1 λ
		P3M0 &= ~(1 << 0);  // ��� P3.0 �� M0 λ

		// P31 - �������������
		P3M1 &= ~(1 << 1);  // ��� P3.1 �� M1 λ
		P3M0 |=  (1 << 1);  // ���� P3.1 �� M0 λ -> �������

		// P32 - I2C SDA
		P3M1 |=  (1 << 2);  // ���� P3.2 �� M1 λ
		P3M0 |=  (1 << 2);  // ���� P3.2 �� M0 λ -> ��©�����I2C Ҫ��

		// P33 - I2C SCL
		P3M1 |=  (1 << 3);  // ���� P3.3 �� M1 λ
		P3M0 |=  (1 << 3);  // ���� P3.3 �� M0 λ -> ��©�����I2C Ҫ��

		// P54 - ������ A ������
		P5M1 &= ~(1 << 4);  // ��� P5.4 �� M1 λ
		P5M0 &= ~(1 << 4);  // ��� P5.4 �� M0 λ -> ׼˫��ڣ����룩

		// P55 - ������ B ������
		P5M1 &= ~(1 << 5);  // ��� P5.5 �� M1 λ
		P5M0 &= ~(1 << 5);  // ��� P5.5 �� M0 λ -> ׼˫��ڣ����룩
}

void main(void)
{

		gpio_init();
//		UartInit(); 
		encoder_init();
		Timer0_Init();
    Buzzer_Pin_Init(); // ��������
    PWM_Init();        // ��ʼ��PWM
		EA = 1; 
		OLED_Init();//��ʼ��OLED
		OLED_ColorTurn(0);//0������ʾ��1 ��ɫ��ʾ
		OLED_DisplayTurn(0);//0������ʾ 1 ��Ļ��ת��ʾ
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

