#include "encoder.h"
#include "uart.h"
#include "oled.h"

#define EX2 0x10
#define EX3 0x20
#define EX4 0x40



volatile int time_h = 0;
volatile int time_m = 0;
volatile int time_s = 0;
volatile char encoder_step = 0;  // +10 À≥ ±’Î£¨-10 ƒÊ ±’Î

volatile bit key_pressed = 0;
enum Mode current_mode = SET_MODE;

void encoder_init(void)
{
	INTCLKO = EX2;
}

void INT2_Isr() interrupt 10
{
    if (current_mode == SET_MODE)
		{
        if (ENCODER_B)
            encoder_step = 10;
        else
            encoder_step = -10;
    }
}
