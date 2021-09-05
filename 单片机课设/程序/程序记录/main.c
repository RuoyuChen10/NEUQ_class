#include <reg52.h>
#include <absacc.h>

sbit DJ_A=P0^0;
sbit DJ_B=P0^1;
sbit DJ_C=P0^2;
sbit DJ_D=P0^3;

int DJ_X = 0;

sbit RUN=P3^2;
sbit K1=P2^0;		//ģʽѡ��
sbit K2=P2^1;		//����
sbit K3=P2^2;		//����
sbit K4=P2^3;		//��
sbit K5=P2^4;		//��
sbit K6=P2^5;		//ȷ��

sbit LSA=P0^4;
sbit LSB=P0^5;
sbit LSC=P0^6;

int LV0 = 0;		//�������ʾ�ٶ�
int LV1 = 6;
int LV2 = 0;
int V0 = 0;			//�ٶ�
int V1 = 6;
int V2 = 0;

int st=200;					//�������˸
int sj = 0;			//�Ƿ����

int direct=0;		//ת���ж�
int direct0=0;
int mode = 0;		//ģʽ
int go = 0;			//�Ƿ�����
int wv=60;			//ѭ��λ

int TH_ = 0xaE;
int TL_ = 0x00;

unsigned char code smgduan[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
					0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};//��ʾ0~F��ֵ



void delay(unsigned int i)
{
	while(i--);	
}

void T0_init(void)
{
	TMOD|=0X0;	//������ʽ1����ʱʹ��
	TH0=TH_;		//��ֵ
	TL0=TL_;	
	ET0=1;
}

void Int0Init()
{
	//����INT0
	IT0=1;//�����س�����ʽ���½��أ�
	EX0=1;//��INT0���ж�����	
}

void SMG_Display()
{
	int i;
	for(i=0;i<6;i++)
	{
		switch(i)	 //λѡ��ѡ�����������ܣ�
		{
			case(0):
				LSA=0;LSB=0;LSC=0; break;//��ʾ��0λ
			case(1):
				LSA=1;LSB=0;LSC=0; break;//��ʾ��1λ
			case(2):
				LSA=0;LSB=1;LSC=0; break;//��ʾ��2λ
			case(3):
				LSA=1;LSB=1;LSC=0; break;//��ʾ��3λ
			case(4):
				LSA=0;LSB=0;LSC=1; break;//��ʾ��4λ
			case(5):
				LSA=1;LSB=0;LSC=1; break;//��ʾ��5λ
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
			delay(500); //���һ��ʱ��ɨ��
			P1=0x00;//����
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
			delay(500); //���һ��ʱ��ɨ��	
			P1=0x00;//����
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

void DATA_get()	//������������
{
	if(K1==0)		  //ģʽת��
	{	
		delay(1000);
		if(K1==0)
		{
			mode = mode+1;
			sj = 0;
		}
		while(!K1);
	}
	if(K6==0)		  //�Ƿ�ı����
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
	
	if((mode%2)==0&&(sj%2)==1)	//�ı���ת��ת
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
	if(mode%2==1&&(sj%2)==1)	//�ı�ת��
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
		if(K4==0)		//�Ӳ���
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
		if(K5==0)		//������
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
	DJ_A=1;				//�̶�ס���
	T0_init();		//���ö�ʱ��0�ж�
	Int0Init();   //�����ⲿ�ж�0
	TR0=0;
	EA = 1;
	while(1){
		DATA_get();
		SMG_Display();
	}
}


void Int0()	interrupt 0		//�ⲿ�ж�0���жϺ���
{
	TR0=~TR0;
	DJ_X=0;
}

void Timer0() interrupt 1
{
	TH0=TH_;	//����ʱ������ֵ����ʱ1ms
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