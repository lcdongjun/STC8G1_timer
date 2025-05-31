#include "IRM.h"
#include "uart.h"

#define THRESHOLD ((unsigned int)(((FOSC * 7UL) + 6000000UL) / 12000000UL))
	
#define irtimeH  (unsigned int)(FOSC / 12000000 * 63)
#define irtimeL  (unsigned int)(FOSC / 12000000 * 33)
	
unsigned char  irtime;
bit irpro_ok,irok;
unsigned int IRcord[4];
unsigned char irdata[33];


void Ir_work(void)//红外键值散转程序
{
		
		if(IRcord[2] == 0x700)
		{
			printf("P54 toggled\n");
			P54=~P54;
		}
		else if(IRcord[2] == 0x900)
		{
			printf("P55 toggled\n");
			P55=~P55;
		}
		else
		{
			printf("irm date: 0x%x",IRcord[2]);
		}
		irpro_ok=0;
}

void Ircordpro(void)//红外码值处理函数
{ 
	unsigned char i, j, k,cord,value;
	k=1;
	for(i=0;i<4;i++)      //处理4个字节
	{
		for(j=1;j<=8;j++) //处理1个字节8位
		{
			cord=irdata[k];
			if (cord > THRESHOLD)
			value=value|0x80;
			if(j<8)
			{
				value>>=1;
			}
			k++;
		}
		IRcord[i]=value;
		value=0;     
	} 
	irpro_ok=1;
}

static void Timer0_Init(void)//定时器0初始化
{
	TMOD=0x02;//定时器0工作方式2，TH0是重装值，TL0是初值
	TH0=0x00; //重载值
	TL0=0x00; //初始化值
	ET0=1;    //开中断
	TR0=1;    
}
static void INT0_Init(void)
{
	IT0 = 1;   //指定外部中断0下降沿触发，INT0 (P3.2)
	EX0 = 1;   //使能外部中断
	EA = 1;    //开总中断
}
void irm_init(void)
{
    Timer0_Init();
		INT0_Init();
}

void EX0_ISR (void) interrupt 0
{
	static unsigned char  i;             //接收红外信号处理
	static bit startflag=0;                //是否开始处理标志位
	if(startflag)                         
	{
		if(irtime<irtimeH&&irtime>=irtimeL)//引导码 TC9012的头码，9ms+4.5ms
            i=0;
    		irdata[i]=irtime;//存储每个电平的持续时间，用于以后判断是0还是1
    		irtime=0;
    		i++;
   			if(i==33)
				{
	  			irok=1;
					i=0;
	  		}
	}
	else
	{
		irtime=0;
		startflag=1;
	}
}

void tim0_isr (void) interrupt 1 using 1
{
  irtime++;
}