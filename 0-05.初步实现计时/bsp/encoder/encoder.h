#ifndef __ENCODER_H
#define __ENCODER_H

#include "config.h"

sbit ENCODER_A = P5^4;
sbit ENCODER_B = P5^5;
sbit KEY = P3^0;

/***********************************编码器旋转值的采集函数*************************************************/
#define ENCODER_DEBOUNCE_MS 10  // 抖动时间阈值

enum Mode { SET_MODE, COUNTDOWN_MODE ,PAUSE_MODE};//按键切换的状态
extern enum Mode current_mode;

//时间
extern volatile int time_h;
extern volatile int time_m;
extern volatile int time_s;

#define SAMPLE_COUNT 5					//重复采样次数
extern volatile long encoder_step;//编码器的计数
extern bit sampling_ready;				//重复采集完成
extern char encoder_samples[SAMPLE_COUNT];//重复采集的值
extern char sample_index;					//当前重复采集的次数

void encoder_init(void);

#endif

