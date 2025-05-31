#include "encoder.h"
#include "uart.h"
#include "oled.h"
#include "timer0.h"


/***************************************编码器按键的采集函数*********************************************************/
#define EX2 0x10
#define EX3 0x20
#define EX4 0x40

char encoder_samples[SAMPLE_COUNT] = {0};
char sample_index = 0;
bit sampling_ready = 0;

volatile int time_h = 0;
volatile int time_m = 0;
volatile int time_s = 0;
volatile long encoder_step = 0;


enum Mode current_mode = SET_MODE;

unsigned long last_encoder_time = 0;  // 上一次采样时间


void encoder_init(void)
{
	INTCLKO = EX2;

}

void INT2_Isr() interrupt 10
{	
		if (current_mode == SET_MODE)
    {
        unsigned long now = getsystick();
        char direction = 0;
        // 非阻塞消抖：必须隔 ENCODER_DEBOUNCE_MS 毫秒才响应一次
        if (now - last_encoder_time < ENCODER_DEBOUNCE_MS)
            return;  // 忽略本次抖动

        last_encoder_time = now;  // 更新上次有效时间

        // 根据B相判断方向
        if (ENCODER_B)
            direction = +1;
        else
            direction = -1;

        // 存入采样缓冲
        encoder_samples[sample_index++] = direction;

        if (sample_index >= SAMPLE_COUNT)
        {
            sample_index = 0;
            sampling_ready = 1;
        }
    }
}

