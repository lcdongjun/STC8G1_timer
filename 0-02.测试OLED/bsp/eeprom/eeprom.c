#include "eeprom.h"
#include "uart.h"

static void IapIdle() 
{ 
    IAP_CONTR = 0;
    IAP_CMD = 0;  
    IAP_TRIG = 0;  
    IAP_ADDRH = 0x80;  
    IAP_ADDRL = 0;  
} 

char IapRead(int addr) 
{ 
    char dat;
    IAP_CONTR = 0x80;  
    IAP_TPS = 12;  
    IAP_CMD = 1;  
    IAP_ADDRL = addr;  
    IAP_ADDRH = addr >> 8;  
    IAP_TRIG = 0x5A;  
    IAP_TRIG = 0xA5;  
    _nop_();  
    dat = IAP_DATA;  
    IapIdle();  
    return dat;  
} 

void IapProgram(int addr, char dat) 
{ 
    char EA_SAVE = EA;  
    EA = 0;

    IAP_CONTR = 0x80;  
    IAP_TPS = 12;  
    IAP_CMD = 2;  
    IAP_ADDRL = addr;  
    IAP_ADDRH = addr >> 8;  
    IAP_DATA = dat;  
    IAP_TRIG = 0x5A;  
    IAP_TRIG = 0xA5;  
    _nop_();  

    IapIdle();  
    EA = EA_SAVE;
} 

void IapErase(int addr) 
{ 
    char EA_SAVE = EA;  
    EA = 0;

    IAP_CONTR = 0x80;  
    IAP_TPS = 12;  
    IAP_CMD = 3;  
    IAP_ADDRL = addr;  
    IAP_ADDRH = addr >> 8;  
    IAP_TRIG = 0x5A;  
    IAP_TRIG = 0xA5;  
    _nop_();  

    IapIdle();  
    EA = EA_SAVE;
} 


void IapSafeWrite(int addr, char dat) 
{
    if (IapRead(addr) != dat) 
		{
        IapProgram(addr, dat);
    }
}



