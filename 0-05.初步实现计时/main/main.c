#include "config.h"
#include "timer0.h"
#include "uart.h"
#include "delay.h"

#include "encoder.h"
#include "beep.h"
#include "oled.h"
#include "bmp.h"

uint8_t xdata time_buf_main[16];// 主界面时间显示缓冲
volatile bit beep_active = 0; // 蜂鸣器开关状态
volatile bit beep_on = 0;
unsigned long t0 = 0, time_update = 0;
void Handle_Buzzer(bit on);
volatile bit WDT_FLAGE = 1; // 看门狗标志位
#define AUTO_SLEEP_TIMEOUT_MS 30000
volatile unsigned long last_user_tick = 0; // 记录上次用户操作时间
//电池没电图标 32*16
const unsigned char code BAT_0[] = {
0x00,0x00,0xFE,0xFE,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0xF6,0xF6,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0xFE,0xFE,0xFC,0xF0,0xE0,0x00,0x00,0x00,0x00,0x7F,0x7F,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x6B,0x6B,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x7F,0x7F,0x3F,0x0F,0x07,0x00,0x00/*电池0.bmp*/
};
//锁定屏幕图标 16*16
const unsigned char code LOCK[] = {
0x00,0x00,0x00,0x80,0xF0,0xF8,0x8C,0x8C,0x8C,0x8C,0xF8,0xF0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x3F,0x3F,0x33,0x3F,0x3F,0x3F,0x3F,0x00,0x00,0x00//锁.bmp
};
volatile bit BAT_FLAGE = 0;

void gpio_init(void)
{
    // P30 - 编码器按下按钮输入
    P3M1 |= (1 << 0);  // 清除 P3.0 的 M1 位
    P3M0 &= ~(1 << 0); // 清除 P3.0 的 M0 位

    // P31 - 蜂鸣器控制输出
    P3M1 &= ~(1 << 1); // 清除 P3.1 的 M1 位
    P3M0 |= (1 << 1);  // 设置 P3.1 的 M0 位 -> 推挽输出
		P3SR &= ~(1 << 1); // 设置 P31 为高速输出
    // P32 - I2C SDA
    P3M1 |= (1 << 2); // 设置 P3.2 的 M1 位
    P3M0 |= (1 << 2); // 设置 P3.2 的 M0 位 -> 开漏输出（I2C 要求）

    // P33 - I2C SCL
    P3M1 |= (1 << 3); // 设置 P3.3 的 M1 位
    P3M0 |= (1 << 3); // 设置 P3.3 的 M0 位 -> 开漏输出（I2C 要求）

    // P54 - 编码器 A 相输入
    P5M1 &= ~(1 << 4); // 清除 P5.4 的 M1 位
    P5M0 &= ~(1 << 4); // 清除 P5.4 的 M0 位 -> 准双向口（输入）

    // P55 - 编码器 B 相输入
    P5M1 &= ~(1 << 5); // 清除 P5.5 的 M1 位
    P5M0 &= ~(1 << 5); // 清除 P5.5 的 M0 位 -> 准双向口（输入）
}
//状态切换
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
    OLED_ShowString(0, 2, time_buf_main, 32); // 字号32，居中显示
		if(current_mode == COUNTDOWN_MODE)
		{
			OLED_DrawBMP(33,0,16,16,LOCK);
		}
}
// 按键切换模式并控制蜂鸣器
void Handle_KeyPress()
{
    static key_mode = 0; // 0: 未按下, 1: 短按, 2: 长按
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
    else // KEY == 0，按键松开
    {
        if (key_last) // 上一轮是按下状态，现在松开
        {
            if (key_mode == 1) // 短按
            {
                UpdateMode();
            }
            else if (key_mode == 2) // 长按
            {
                PWM_Set(500, 50);
                PWM_ON();
                Delay_ms(50);
                PWM_OFF();
                WDT_FLAGE = 0;//禁止喂狗，实现重启的效果
            }

            // 重置所有状态
            key_mode = 0;
            key_last = 0;
        }
    }
}

// 旋转编码器设置时间
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
            encoder_step = 0; // 干扰忽略

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

// 倒计时逻辑
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
            t0 = getsystick(); // 初次开启蜂鸣器，记录起始时间
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
            t0 = getsystick(); // 重置时间，重新开始一轮节奏

        PWM_ON(); // 确保始终保持开启
    }
    else
    {
        PWM_OFF();
        beep_active = 0;
        t0 = getsystick(); // 清零计时，防止脉冲残留
    }
}

void Check_Inactivity_Sleep(void)
{
    if (current_mode != COUNTDOWN_MODE) // 倒计时模式不自动关机
    {
        if (getsystick() - last_user_tick >= AUTO_SLEEP_TIMEOUT_MS)
        {
            OLED_Clear(); // 可选：关屏
            PWM_OFF();    // 可选：关闭蜂鸣器

            PCON |= 0x02; // 设置掉电位
            _nop_();      // 掉电后立即进入中断
            _nop_();

            gpio_init();
            BEEP = 0;
            encoder_init();
            Timer0_Init();
            OLED_Init();         // 初始化OLED
            OLED_ColorTurn(0);   // 0正常显示，1 反色显示
            OLED_DisplayTurn(0); // 0正常显示 1 屏幕翻转显示
            PWM_Set(1200, 50);
            last_user_tick = getsystick(); // 防止刚唤醒又进入掉电
            EA = 1;                        // 唤醒后重新开启中断
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
    OLED_Init();         // 初始化OLED
    OLED_ColorTurn(0);   // 0正常显示，1 反色显示
    OLED_DisplayTurn(0); // 0正常显示 1 屏幕翻转显示
    sprintf(time_buf_main, "%02d:%02d:%02d", time_h, time_m, time_s);
    OLED_ShowString(0, 2, time_buf_main, 32); // 字号32，居中显示
    PWM_Set(1200, 50);
    WDT_CONTR = 0x24;//使能看门狗,溢出时间约为 1s
    WDT_FLAGE = 1; // 清除看门狗标志位
    RSTCFG = LVD3V0; // 设置 LVD 阈值为 3.0V，不使能复位
    ELVD = 1;        // 开启 LVD 中断
		BAT_FLAGE = 0;   //清除电池没电标志位
    EA = 1;
    time_update = getsystick();
    while (1)
    {
        if(WDT_FLAGE) // 检查看门狗标志
        {
            WDT_CONTR |= 0x10; //清看门狗
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
    PCON &= ~LVDF; // 清中断标志
		BAT_FLAGE = 1;
}