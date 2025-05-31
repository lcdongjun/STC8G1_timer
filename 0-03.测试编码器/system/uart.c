#include "uart.h"

#define BRT (65536 - FOSC / 115200 / 4) 

 
bit busy; 
char wptr; 
char rptr; 
char buffer[16]; 


void UartInit() 
{ 
	//串口1使用定时器1（模式0）做波特率发生器
	SCON = 0x50;
	TMOD = 0x00;
	TL1 = BRT;
	TH1 = BRT >> 8;
	TR1 = 1;
	AUXR |= (1<<6);
	wptr = 0x00;
	rptr = 0x00;
	busy = 0;
} 
void UartSend(char dat) 
{ 
	while (busy); 
	busy = 1; 
	SBUF = dat; 
} 
void UartSendStr(char *p) 
{ 
	while (*p) 
	{ 
		UartSend(*p++); 
	} 
}


void UartIsr() interrupt 4  
{ 
	if (TI) 
	{ 
		TI = 0; 
		busy = 0; 
	} 
	if (RI) 
	{ 
		RI = 0; 
		buffer[wptr++] = SBUF; 
		wptr &= 0x0f; 
	} 
}

char putchar(char ch)
{
    SBUF = ch;
    while(TI == 0);
    TI = 0;
    return ch;
}
