#include "beep.h"
#include "uart.h"

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

// ���ŵ�������
void Beep_Note(uint16_t freq, uint8_t volume, uint16_t duration_ms)
{
    Buzzer_Set(freq, volume);
    Delay_ms(duration_ms);
    Buzzer_Set(0, 0); // �ر�
    Delay_ms(10);      // �������
}

// �򵥲���һ�����֣���������
void Play_Melody(void)
{
    // �� C����do re mi so��
    Beep_Note(523, 90, 300);  // Do
    Beep_Note(587, 90, 300);  // Re
    Beep_Note(659, 90, 300);  // Mi
    Beep_Note(784, 90, 300);  // So
    Beep_Note(659, 90, 300);  // Mi
    Beep_Note(784, 90, 600);  // So
		Delay_ms(500);
}

