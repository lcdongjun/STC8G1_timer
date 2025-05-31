#include "config.h"
#include "uart.h"


typedef unsigned char      uint8_t;
typedef signed char        int8_t;
typedef unsigned int       uint16_t;
typedef signed int         int16_t;
typedef unsigned long      uint32_t;
typedef signed long        int32_t;

// ���� P3.1 Ϊ������� + ӳ�� CCP0 �� P3.1
void Buzzer_Pin_Init(void)
{
    P3M1 &= ~(1 << 1);
    P3M0 |=  (1 << 1);     // P3.1 �������
    P_SW1 |= 0x10;         // ӳ�� CCP0 -> CCP0_2 (P3.1)
}

// ��ʼ�� PWM �����PCA + PWM ģ��0��CCP0��
void PWM_Init(void)
{
    CCON = 0x00;           // PCA �������
    CMOD = 0x00;           // PCAʱ�� = ϵͳʱ�� / 128
    CL = 0x00;             // ��� PCA ������
    CH = 0x00;

    CCAPM0 = 0x42;         // ģ��0: PWM ģʽ + �Ƚ�ʹ��
    PCA_PWM0 = 0x00;       // 8λPWM
    CR = 1;                // ���� PCA
}

// ���÷��������Ƶ�ʺ�����
// freq: Ƶ�ʣ�Hz����volume: ռ�ձȣ�1~99���ٷֱȣ�
void Buzzer_Set(uint16_t freq, uint8_t volume)
{
    uint32_t pwm_clock = FOSC / 128; // PCAʱ�� = ϵͳʱ��/128
    uint32_t period;
    uint8_t duty;

    if(freq == 0 || volume == 0) {
        CCAP0H = 0xFF;  // ռ�ձ� 0%������
        CCAP0L = 0xFF;
        return;
    }

    period = pwm_clock / freq;
    if(period > 256) period = 256;   // �����������
    if(period < 10)  period = 10;    // ��ֹƵ��̫��

    duty = period * (100 - volume) / 100;

    CCAP0H = duty;
    CCAP0L = duty;
}

// ��ʱ��������λ ms��
void DelayMs(uint16_t ms)
{
    uint16_t i, j;
    for(i = ms; i > 0; i--)
        for(j = 110; j > 0; j--);
}

// ���ŵ�������
void Beep_Note(uint16_t freq, uint8_t volume, uint16_t duration_ms)
{
    Buzzer_Set(freq, volume);
    DelayMs(duration_ms);
    Buzzer_Set(0, 0); // �ر�
    DelayMs(10);      // �������
}

// �򵥲���һ�����֣���������
void Play_Melody(void)
{
    // �� C����do re mi so��
    Beep_Note(523, 90, 300);  // Do
	    DelayMs(500);
    Beep_Note(587, 90, 300);  // Re
	    DelayMs(500);
    Beep_Note(659, 90, 300);  // Mi
	    DelayMs(500);
    Beep_Note(784, 90, 300);  // So
	    DelayMs(500);
    Beep_Note(659, 90, 300);  // Mi
	    DelayMs(500);
    Beep_Note(784, 90, 600);  // So
      DelayMs(500);
}

void main(void)
{
    Buzzer_Pin_Init(); // ��������
    PWM_Init();        // ��ʼ��PWM

    while(1)
    {
        Play_Melody(); // ����һ������
        DelayMs(1000);
    }
}