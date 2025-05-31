#ifndef __UART_H
#define __UART_H

#include "config.h"


extern bit busy; 
extern char wptr; 
extern char rptr; 
extern char buffer[16]; 


void UartInit(void);
void UartSend(char dat);
void UartSendStr(char *p);


#endif
