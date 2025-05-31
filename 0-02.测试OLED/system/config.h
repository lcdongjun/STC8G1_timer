#ifndef __CONFIGT_H
#define __CONFIG_H

#include <STC8G.h>
#include <stdio.h>
#include <intrins.h>

#include "delay.h"

#define uint8_t     unsigned char
#define int8_t      signed char
#define uint16_t    unsigned int
#define int16_t     signed int
#define uint32_t    unsigned long
#define int32_t     signed long

#define FOSC		24000000L	//定义主时钟
//#define FOSC		22118400L	//定义主时钟
//#define FOSC		12000000L	//定义主时钟
//#define FOSC		11059200L	//定义主时钟
//#define FOSC		 5529600L	//定义主时钟

#define DS18B20_PIN P33	// DS18B20 接收器连接到 P3.2
#define IR_PIN P32  		// 1838 接收器连接到 P3.2

#endif
