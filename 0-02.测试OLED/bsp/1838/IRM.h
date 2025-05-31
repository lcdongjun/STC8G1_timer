#ifndef __IRM_H
#define __IRM_H

#include "config.h"


extern unsigned char  irtime;	//红外用全局变量

extern bit irpro_ok,irok;
extern unsigned int IRcord[4];
extern unsigned char irdata[33];

void irm_init(void);
void Ircordpro(void);
void Ir_work(void);

#endif

