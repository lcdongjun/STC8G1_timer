#include "config.h"
#include "timer0.h"
#include "uart.h"
#include "delay.h"

#include "encoder.h"
#include "beep.h"
#include "oled.h"
#include "bmp.h"

uint8_t xdata time_buf_main[16];// ������ʱ����ʾ����
volatile bit beep_active = 0; // ����������״̬
volatile bit beep_on = 0;
unsigned long t0 = 0, time_update = 0;
void Handle_Buzzer(bit on);
volatile bit WDT_FLAGE = 1; // ���Ź���־λ
#define AUTO_SLEEP_TIMEOUT_MS 30000
volatile unsigned long last_user_tick = 0; // ��¼�ϴ��û�����ʱ��
//���û��ͼ�� 32*16
const unsigned char code BAT_0[] = {
0x00,0x00,0xFE,0xFE,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0xF6,0xF6,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0xFE,0xFE,0xFC,0xF0,0xE0,0x00,0x00,0x00,0x00,0x7F,0x7F,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x6B,0x6B,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x7F,0x7F,0x3F,0x0F,0x07,0x00,0x00/*���0.bmp*/
};
//������Ļͼ�� 16*16
const unsigned char code LOCK[] = {
0x00,0x00,0x00,0x80,0xF0,0xF8,0x8C,0x8C,0x8C,0x8C,0xF8,0xF0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x3F,0x3F,0x33,0x3F,0x3F,0x3F,0x3F,0x00,0x00,0x00//��.bmp
};
volatile bit BAT_FLAGE = 0;

void gpio_init(void)
{
    // P30 - ���������°�ť����
    P3M1 |= (1 << 0);  // ��� P3.0 �� M1 λ
    P3M0 &= ~(1 << 0); // ��� P3.0 �� M0 λ

    // P31 - �������������
    P3M1 &= ~(1 << 1); // ��� P3.1 �� M1 λ
    P3M0 |= (1 << 1);  // ���� P3.1 �� M0 λ -> �������
		P3SR &= ~(1 << 1); // ���� P31 Ϊ�������
    // P32 - I2C SDA
    P3M1 |= (1 << 2); // ���� P3.2 �� M1 λ
    P3M0 |= (1 << 2); // ���� P3.2 �� M0 λ -> ��©�����I2C Ҫ��

    // P33 - I2C SCL
    P3M1 |= (1 << 3); // ���� P3.3 �� M1 λ
    P3M0 |= (1 << 3); // ���� P3.3 �� M0 λ -> ��©�����I2C Ҫ��

    // P54 - ������ A ������
    P5M1 &= ~(1 << 4); // ��� P5.4 �� M1 λ
    P5M0 &= ~(1 << 4); // ��� P5.4 �� M0 λ -> ׼˫��ڣ����룩

    // P55 - ������ B ������
    P5M1 &= ~(1 << 5); // ��� P5.5 �� M1 λ
    P5M0 &= ~(1 << 5); // ��� P5.5 �� M0 λ -> ׼˫��ڣ����룩
}
//״̬�л�
void UpdateMode(void)
{

    current_mode = (current_mode == SET_MODE) ? COUNTDOWN_MODE : SET_MODE;
    last_user_tick = getsystick();
    OLED_Clear();
    PWM_Set(1200, 50);
    PWM_ON();
    Delay_ms(100);
    PWM_OFF();
    beep_on = 0;
    sprintf(time_buf_main, "%02d:%02d:%02d", time_h, time_m, time_s);
    OLED_ShowString(0, 2, time_buf_main, 32); // �ֺ�32��������ʾ
		if(current_mode == COUNTDOWN_MODE)
		{
			OLED_DrawBMP(33,0,16,16,LOCK);
		}
}
// �����л�ģʽ�����Ʒ�����
void Handle_KeyPress()
{
    static key_mode = 0; // 0: δ����, 1: �̰�, 2: ����
    static bit key_last = 0;
    static unsigned long key_down_time = 0;

    if (KEY == 1)
    {
        if (!key_last)
        {
            key_last = 1;
            key_down_time = getsystick();
        }
        else
        {
            unsigned long duration = getsystick() - key_down_time;
            if (duration >= 1400)
            {
                key_mode = 2;
            }
            if (duration >= 200 && duration <= 1400)
            {
                key_mode = 1;
            }
        }
    }
    else // KEY == 0�������ɿ�
    {
        if (key_last) // ��һ���ǰ���״̬�������ɿ�
        {
            if (key_mode == 1) // �̰�
            {
                UpdateMode();
            }
            else if (key_mode == 2) // ����
            {
                PWM_Set(500, 50);
                PWM_ON();
                Delay_ms(50);
                PWM_OFF();
                WDT_FLAGE = 0;//��ֹι����ʵ��������Ч��
            }

            // ��������״̬
            key_mode = 0;
            key_last = 0;
        }
    }
}

// ��ת����������ʱ��
void Handle_Encoder()
{
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
            encoder_step = 0; // ���ź���

        sampling_ready = 0;
    }
    if (current_mode == SET_MODE && encoder_step != 0)
    {
        last_user_tick = getsystick();
        PWM_Set(1400, 50);
        PWM_ON();
        Delay_ms(50);
        PWM_OFF();
        UpdateTime(encoder_step * 30);
        encoder_step = 0;
    }
}

// ����ʱ�߼�
void Handle_Countdown()
{
    if (current_mode == COUNTDOWN_MODE && getsystick() - time_update >= 1000)
    {
        time_update = getsystick();
        UpdateTime(-1);
        if (time_h == 0 && time_m == 0 && time_s == 1 && !beep_active)
        {
            beep_on = 1;
        }
    }
}

const uint16_t nokia_melody[] = {
    1319, 1175, 1047, 1175, 1319, 1319, 1319,
    1175, 1175, 1175, 1319, 1568, 1568};

// ����������
void Handle_Buzzer(bit on)
{
    static unsigned long t0 = 0;
    unsigned long elapsed;

    if (on)
    {
        if (!beep_active)
        {
            beep_active = 1;
            t0 = getsystick(); // ���ο�������������¼��ʼʱ��
        }

        elapsed = getsystick() - t0;

        if (elapsed < 200)
            PWM_Set(nokia_melody[0], 50);
        else if (elapsed < 400)
            PWM_Set(nokia_melody[1], 50);
        else if (elapsed < 600)
            PWM_Set(nokia_melody[2], 50);
        else if (elapsed < 800)
            PWM_Set(nokia_melody[3], 50);
        else if (elapsed < 1000)
            PWM_Set(nokia_melody[4], 50);
        else if (elapsed < 1200)
            PWM_Set(nokia_melody[5], 50);
        else if (elapsed < 1400)
            PWM_Set(nokia_melody[6], 50);
        else if (elapsed < 1800)
            PWM_Set(nokia_melody[7], 50);
        else if (elapsed < 2000)
            PWM_Set(nokia_melody[8], 50);
        else if (elapsed < 2200)
            PWM_Set(nokia_melody[9], 50);
        else if (elapsed < 2400)
            PWM_Set(nokia_melody[10], 50);
        else if (elapsed < 2600)
            PWM_Set(nokia_melody[11], 50);
        else if (elapsed < 2800)
            PWM_Set(nokia_melody[12], 50);

        else
            t0 = getsystick(); // ����ʱ�䣬���¿�ʼһ�ֽ���

        PWM_ON(); // ȷ��ʼ�ձ��ֿ���
    }
    else
    {
        PWM_OFF();
        beep_active = 0;
        t0 = getsystick(); // �����ʱ����ֹ�������
    }
}

void Check_Inactivity_Sleep(void)
{
    if (current_mode != COUNTDOWN_MODE) // ����ʱģʽ���Զ��ػ�
    {
        if (getsystick() - last_user_tick >= AUTO_SLEEP_TIMEOUT_MS)
        {
            OLED_Clear(); // ��ѡ������
            PWM_OFF();    // ��ѡ���رշ�����

            PCON |= 0x02; // ���õ���λ
            _nop_();      // ��������������ж�
            _nop_();

            gpio_init();
            BEEP = 0;
            encoder_init();
            Timer0_Init();
            OLED_Init();         // ��ʼ��OLED
            OLED_ColorTurn(0);   // 0������ʾ��1 ��ɫ��ʾ
            OLED_DisplayTurn(0); // 0������ʾ 1 ��Ļ��ת��ʾ
            PWM_Set(1200, 50);
            last_user_tick = getsystick(); // ��ֹ�ջ����ֽ������
            EA = 1;                        // ���Ѻ����¿����ж�
        }
    }
}
#define LVD3V0 0x03 // LVD@3.0V
#define LVDF 0x20

void main(void)
{
		gpio_init();
    BEEP = 0;
    encoder_init();
    Timer0_Init();
    OLED_Init();         // ��ʼ��OLED
    OLED_ColorTurn(0);   // 0������ʾ��1 ��ɫ��ʾ
    OLED_DisplayTurn(0); // 0������ʾ 1 ��Ļ��ת��ʾ
    sprintf(time_buf_main, "%02d:%02d:%02d", time_h, time_m, time_s);
    OLED_ShowString(0, 2, time_buf_main, 32); // �ֺ�32��������ʾ
    PWM_Set(1200, 50);
    WDT_CONTR = 0x24;//ʹ�ܿ��Ź�,���ʱ��ԼΪ 1s
    WDT_FLAGE = 1; // ������Ź���־λ
    RSTCFG = LVD3V0; // ���� LVD ��ֵΪ 3.0V����ʹ�ܸ�λ
    ELVD = 1;        // ���� LVD �ж�
		BAT_FLAGE = 0;   //������û���־λ
    EA = 1;
    time_update = getsystick();
    while (1)
    {
        if(WDT_FLAGE) // ��鿴�Ź���־
        {
            WDT_CONTR |= 0x10; //�忴�Ź�
        }
        Handle_KeyPress();
        Handle_Encoder();
        Handle_Countdown();
        Handle_Buzzer(beep_on);
        Check_Inactivity_Sleep();
        ShowTime();
				if(BAT_FLAGE)
					OLED_DrawBMP(0,0,32,16,BAT_0);
    }
}

void Lvd_Isr() interrupt 6
{
    PCON &= ~LVDF; // ���жϱ�־
		BAT_FLAGE = 1;
}