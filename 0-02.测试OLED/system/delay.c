#include "delay.h"

void Delay_us(unsigned int us) 
{
    while (us--) {
        _nop_(); _nop_(); _nop_(); _nop_(); //   ”√”⁄ 24MHz
        _nop_(); _nop_(); _nop_(); _nop_();
        _nop_(); _nop_(); _nop_(); _nop_();
				_nop_();
    }
}

void Delay_ms(unsigned int ms) 
{
    while (ms--) {
        Delay_us(1000);
    }
}

