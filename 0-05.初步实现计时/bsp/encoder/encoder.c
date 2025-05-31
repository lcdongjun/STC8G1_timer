#include "encoder.h"
#include "uart.h"
#include "oled.h"
#include "timer0.h"


/***************************************�����������Ĳɼ�����*********************************************************/
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

unsigned long last_encoder_time = 0;  // ��һ�β���ʱ��


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
        // ����������������� ENCODER_DEBOUNCE_MS �������Ӧһ��
        if (now - last_encoder_time < ENCODER_DEBOUNCE_MS)
            return;  // ���Ա��ζ���

        last_encoder_time = now;  // �����ϴ���Чʱ��

        // ����B���жϷ���
        if (ENCODER_B)
            direction = +1;
        else
            direction = -1;

        // �����������
        encoder_samples[sample_index++] = direction;

        if (sample_index >= SAMPLE_COUNT)
        {
            sample_index = 0;
            sampling_ready = 1;
        }
    }
}

