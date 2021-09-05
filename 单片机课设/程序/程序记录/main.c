#include <reg52.h>
#include <absacc.h>

sbit DJ_A=P0^0;
sbit DJ_B=P0^1;
sbit DJ_C=P0^2;
sbit DJ_D=P0^3;

int DJ_X = 0;

sbit RUN=P3^2;
sbit K1=P2^0;		//模式选择
sbit K2=P2^1;		//左移
sbit K3=P2^2;		//右移
sbit K4=P2^3;		//加
sbit K5=P2^4;		//减
sbit K6=P2^5;		//确定

sbit LSA=P0^4;
sbit LSB=P0^5;
sbit LSC=P0^6;

int LV0 = 0;		//数码管显示速度
int LV1 = 6;
int LV2 = 0;
int V0 = 0;			//速度
int V1 = 6;
int V2 = 0;

int st=200;					//数码管闪烁
int sj = 0;			//是否调数

int direct=0;		//转向判断
int direct0=0;
int mode = 0;		//模式
int go = 0;			//是否启动
int wv=60;			//循环位

int TH_ = 0xaE;
int TL_ = 0x00;

unsigned char code smgduan[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
					0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};//显示0~F的值



void delay(unsigned int i)
{
	while(i--);	
}

void T0_init(void)
{
	TMOD|=0X0;	//工作方式1，计时使用
	TH0=TH_;		//初值
	TL0=TL_;	
	ET0=1;
}

void Int0Init()
{
	//设置INT0
	IT0=1;//跳变沿出发方式（下降沿）
	EX0=1;//打开INT0的中断允许。	
}

void SMG_Display()
{
	int i;
	for(i=0;i<6;i++)
	{
		switch(i)	 //位选，选择点亮的数码管，
		{
			case(0):
				LSA=0;LSB=0;LSC=0; break;//显示第0位
			case(1):
				LSA=1;LSB=0;LSC=0; break;//显示第1位
			case(2):
				LSA=0;LSB=1;LSC=0; break;//显示第2位
			case(3):
				LSA=1;LSB=1;LSC=0; break;//显示第3位
			case(4):
				LSA=0;LSB=0;LSC=1; break;//显示第4位
			case(5):
				LSA=1;LSB=0;LSC=1; break;//显示第5位
		}
		if(mode%2==0){
			switch(i){
				case(5):if(sj%2==0||st<100)P1=smgduan[direct%2];else P1=0x00; break;
				case(4):P1=0x00;break;
				case(3):P1=0x00;break;
				case(2):P1=0x00;break;
				case(1):P1=0x00;break;
				case(0):P1=0x5E;break;
			}
			delay(500); //间隔一段时间扫描
			P1=0x00;//消隐
			if(sj%2==1)
				st = st-1;
			if(st==0){st = 200;}
		}
		if(mode%2==1){
			switch(i){
				case(5):if(sj%2==1&&(st>=100)&&(wv%3==0)) P1=0x00;else P1=smgduan[LV0];break;
				case(4):if(sj%2==1&&(st>=100)&&(wv%3==1)) P1=0x00;else P1=smgduan[LV1];break;
				case(3):if(sj%2==1&&(st>=100)&&(wv%3==2)) P1=0x00;else P1=smgduan[LV2];break;
				case(2):P1=0x00;break;
				case(1):P1=0x00;break;
				case(0):P1=0x3E;break;
			}
			delay(500); //间隔一段时间扫描	
			P1=0x00;//消隐
			if(sj%2==1)
				st = st-1;
			if(st==0){st = 200;}
		}
	}
}

void calculate_time()
{
	int v,x;
	v = V2*100+V1*10+V0;
	x = 65536-60/12/v/200*11059200;
	if(x<256)
	{
		TH_=0;
		TL_=x;
	}
	else
	{
		TL_=255;
		TH_=(x-255)/256;
	}
}

void DATA_get()	//按键输入数据
{
	if(K1==0)		  //模式转换
	{	
		delay(1000);
		if(K1==0)
		{
			mode = mode+1;
			sj = 0;
		}
		while(!K1);
	}
	if(K6==0)		  //是否改变参数
	{	
		delay(1000);
		if(K6==0)
		{
			sj = sj+1;
		}
		if(sj%2==0&&(mode%2==1))
		{
			V0=LV0;
			V1=LV1;
			V2=LV2;
			calculate_time();
		}
		else if(sj%2==0&&(mode%2==0)){
			direct0 = direct;
			DJ_D = 1;
		}
		while(!K6);
	}
	
	if((mode%2)==0&&(sj%2)==1)	//改变正转反转
	{
		if(K4==0)
		{
			delay(1000);
			if(K4==0)
			{
				direct = direct+1;
			}
			while(!K4);
		}
		if(K5==0)
		{
			delay(1000);
			if(K5==0)
			{
				direct = direct+1;
			}
			while(!K5);
		}
	}
	if(mode%2==1&&(sj%2)==1)	//改变转速
	{
		if(K2==0)
		{
			delay(1000);
			if(K2==0)
			{
				wv=wv+1;
			}
			while(!K2);
		}
		if(K3==0)
		{
			delay(1000);
			if(K3==0)
			{
				wv=wv-1;
			}
			while(!K3);
		}
		if(K4==0)		//加操作
		{
			delay(1000);
			if(K4==0)
			{
				if(wv%3==0){
					LV0=LV0+1;
					if(LV0==10)
						LV0=0;
				}
				if(wv%3==1){
					LV1=LV1+1;
					if(LV1==10)
						LV0=1;
				}
				if(wv%3==2){
					LV2=LV2+1;
					if(LV2==10)
						LV2=1;
				}
			}
			while(!K4);
		}
		if(K5==0)		//减操作
		{
			delay(1000);
			if(K5==0)
			{
				if(wv%3==0){
					LV0=LV0-1;
					if(LV0==-1)
						LV0=9;
				}
				if(wv%3==1){
					LV1=LV1-1;
					if(LV1==-1)
						LV0=9;
				}
				if(wv%3==2){
					LV2=LV2-1;
					if(LV2==-1)
						LV2=9;
				}
			}
			while(!K5);
		}
	}
}



void main()
{	
	DJ_A=1;				//固定住电机
	T0_init();		//设置定时器0中断
	Int0Init();   //设置外部中断0
	TR0=0;
	EA = 1;
	while(1){
		DATA_get();
		SMG_Display();
	}
}


void Int0()	interrupt 0		//外部中断0的中断函数
{
	TR0=~TR0;
	DJ_X=0;
}

void Timer0() interrupt 1
{
	TH0=TH_;	//给定时器赋初值，定时1ms
	TL0=TL_;
	if(direct0%2==0){
		switch(DJ_X){
			case(0):DJ_A=1;DJ_B=0;DJ_C=0;DJ_D=0;break;
			case(1):DJ_A=0;DJ_B=1;DJ_C=0;DJ_D=0;break;
			case(2):DJ_A=0;DJ_B=0;DJ_C=1;DJ_D=0;break;
			case(3):DJ_A=0;DJ_B=0;DJ_C=0;DJ_D=1;break;
		}
	}
	else{
		switch(DJ_X){
			case(0):DJ_A=0;DJ_B=0;DJ_C=0;DJ_D=1;break;
			case(1):DJ_A=0;DJ_B=0;DJ_C=1;DJ_D=0;break;
			case(2):DJ_A=0;DJ_B=1;DJ_C=1;DJ_D=0;break;
			case(3):DJ_A=1;DJ_B=0;DJ_C=0;DJ_D=0;break;
		}
	}
	DJ_X = (DJ_X + 1)%4;
}