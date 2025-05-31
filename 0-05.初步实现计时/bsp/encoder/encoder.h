#ifndef __ENCODER_H
#define __ENCODER_H

#include "config.h"

sbit ENCODER_A = P5^4;
sbit ENCODER_B = P5^5;
sbit KEY = P3^0;

/***********************************��������תֵ�Ĳɼ�����*************************************************/
#define ENCODER_DEBOUNCE_MS 10  // ����ʱ����ֵ

enum Mode { SET_MODE, COUNTDOWN_MODE ,PAUSE_MODE};//�����л���״̬
extern enum Mode current_mode;

//ʱ��
extern volatile int time_h;
extern volatile int time_m;
extern volatile int time_s;

#define SAMPLE_COUNT 5					//�ظ���������
extern volatile long encoder_step;//�������ļ���
extern bit sampling_ready;				//�ظ��ɼ����
extern char encoder_samples[SAMPLE_COUNT];//�ظ��ɼ���ֵ
extern char sample_index;					//��ǰ�ظ��ɼ��Ĵ���

void encoder_init(void);

#endif

