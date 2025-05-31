#include "config.h"
#include "uart.h"
#include "delay.h"

#include "beep.h"
#include "oled.h"
#include "bmp.h"


uint8_t t=' ';
void main(void)
{
		P3M1 &= ~(1 << 1);  // 清除 P3.1 的 M1 位
		P3M0 |=  (1 << 1);  // 设置 P3.1 的 M0 位 -> 推挽输出
	

	// P32 - I2C SDA
		P3M1 |=  (1 << 2);  // 设置 P3.2 的 M1 位
		P3M0 |=  (1 << 2);  // 设置 P3.2 的 M0 位 -> 开漏输出（I2C 要求）

		// P33 - I2C SCL
		P3M1 |=  (1 << 3);  // 设置 P3.3 的 M1 位
		P3M0 |=  (1 << 3);  // 设置 P3.3 的 M0 位 -> 开漏输出（I2C 要求）
//    Buzzer_Pin_Init(); // 配置引脚
//    PWM_Init();        // 初始化PWM
		UartInit(); 
		OLED_Init();//初始化OLED
		OLED_ColorTurn(0);//0正常显示，1 反色显示
		OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
		printf("system ok! \r\n");
		while(1) 
		{		
			//	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel

//			OLED_DrawBMP(0,0,128,64,BMP1);
//			delay_ms(500);
//			OLED_Clear();
//			OLED_ShowChinese(0,0,0,16);//中
//			OLED_ShowChinese(18,0,1,16);//景
//			OLED_ShowChinese(36,0,2,16);//园
//			OLED_ShowChinese(54,0,3,16);//电
//			OLED_ShowChinese(72,0,4,16);//子
//			OLED_ShowChinese(90,0,5,16);//科
//			OLED_ShowChinese(108,0,6,16);//技
//			OLED_ShowString(8,2,"ZHONGJINGYUAN",16);
//			OLED_ShowString(20,4,"2014/05/01",16);
			OLED_ShowString(0,2,"13:05:43",32);
//			OLED_ShowString(0,6,"ASCII:",16);  
//			OLED_ShowString(63,6,"CODE:",16);
//			OLED_ShowChar(48,6,t,16);
//			t++;
//			if(t>'~')t=' ';
//			OLED_ShowNum(103,6,t,3,16);
//			delay_ms(500);
//			OLED_Clear();
		}	  
}