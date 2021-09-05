#include <reg52.h>
#include <absacc.h>
#include<intrins.h>
#include"temp.h"
#define Contadd XBYTE[0xff23] 		//8255���Ƶ�ַ
#define PA XBYTE[0xff20]        	//��ѡ�źŵ�ַ
#define PB XBYTE[0xff21] 	 	//Ƭѡ�źŵ�ַ 
#define adc XBYTE [0xFF80] 		//Ƭѡָ�룬A/Dת����·Ƭ���ַ
#define uchar unsigned char		//�궨��	
#define uint unsigned int
typedef unsigned int u16;	  //���������ͽ�����������
typedef unsigned char u8;	

sbit DJ_A=P1^0;		//�����4����λ
sbit DJ_B=P1^1;
sbit DJ_C=P1^2;
sbit DJ_D=P1^3;

int DJ_X = 0;		//�������ж�
long int JD = 0;	//����Ƕ�

sbit RUN=P3^2;		//��ͣ����
sbit K1=P1^4;			//ģʽѡ��
sbit K2=P1^5;			//����
sbit K3=P1^6;			//����
sbit K4=P1^7;			//��
sbit K5=P3^0;			//��
sbit K6=P3^1;			//ȷ��

int LV0 = 0;			//�������ʾ�ٶ�
int V0 = 0;				//�ٶ�

int st=200;				//�������˸
int sj = 0;				//�Ƿ����

int direct=0;			//ת���ж�
int direct0=0;
int mode = 0;			//ģʽ
int go = 0;				//�Ƿ�����

long int JDC=0;
int JDC_W = 1;		//�Ƕ���ת����
int JCC_LED1=0;		//����Ƕ�LED��ʾ
int JCC_LED2=0;
int JCC_LED3=0;
int JCC_LED4=0;
int JCC_LED5=0;
int wv=60;				//ѭ��λ

char num=0;
unsigned char DisplayData[4];	//�¶�����
int T_judge=0;								//�¶��˲�

int TH_ = 0x4C;		//���ó�����0.02s
int TL_ = 0x00;

unsigned char code smgduan[17]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,
				0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};//��ʾ0~F��ֵ


/*******************************************************************************
* �� �� �� 				: delay
* ��������				: ��ʱ������i=1ʱ����Լ��ʱ10us
*******************************************************************************/
void delay(unsigned int i)
{
	while(i--);	
}

/*******************************************************************************
* �� �� ��     		: T0_init
* ��������				: ��ʱ��T0��ʼ����������ʱ0.02s
*******************************************************************************/
void T0_init(void)
{
	TMOD|=0X0;	//������ʽ1����ʱʹ��
	TH0=TH_;	//��ֵ
	TL0=TL_;	
	ET0=1;
}

/*******************************************************************************
* �� �� ��        : Int0Init
* ��������		   	: �ⲿ�ж�INT0��ʼ�����½��ش���
*******************************************************************************/
void Int0Init()
{
	//����INT0
	IT0=1;//�����س�����ʽ���½��أ�
	EX0=1;//��INT0���ж�����	
}

/*******************************************************************************
* �� �� ��        : datapros
* ��������		   	: �¶ȶ�ȡת������
* ����						: �¶���ֵtemp
*******************************************************************************/
void datapros(int temp) 	 
{
   	float tp;  
	if(temp< 0)				//���¶�ֵΪ����
  	{
		DisplayData[0] = 0xBF; 	  //   -
		//��Ϊ��ȡ���¶���ʵ���¶ȵĲ��룬���Լ�1����ȡ�����ԭ��
		temp=temp-1;
		temp=~temp;
		tp=temp;
		temp=tp*0.0625*100+0.5;	
		//������С�����*100��+0.5���������룬��ΪC���Ը�����ת��Ϊ���͵�ʱ���С����
		//��������Զ�ȥ���������Ƿ����0.5����+0.5֮�����0.5�ľ��ǽ�1�ˣ�С��0.5�ľ�
		//�����0.5��������С������档
 
  	}
 	else
  	{			
		DisplayData[0] = 0xFF;
		tp=temp;//��Ϊ���ݴ�����С�������Խ��¶ȸ���һ�������ͱ���
		//����¶���������ô����ô������ԭ����ǲ���������
		temp=tp*0.0625*100+0.5;	
		//������С�����*100��+0.5���������룬��ΪC���Ը�����ת��Ϊ���͵�ʱ���С����
		//��������Զ�ȥ���������Ƿ����0.5����+0.5֮�����0.5�ľ��ǽ�1�ˣ�С��0.5�ľ�
		//�����0.5��������С������档
	}
	DisplayData[0] = smgduan[temp / 10000];
	DisplayData[1] = smgduan[temp % 10000 / 1000];
	DisplayData[2] = smgduan[temp % 1000 / 100] & 0x7F;
	DisplayData[3] = smgduan[temp % 100 / 10];
	if(temp/10>=315){				//���±���
		T_judge+=1;
	}
	else T_judge=0;
	if(T_judge>10){
		TR0 = 0;
		DJ_A=1;
		//PB &= 0x7F;
	}
}

/*******************************************************************************
* �� �� ��        : SMG_Display
* ��������		   	: �������ʾ������������ʾ5��ģʽ����1����2�ٶȵ�λ��3�Ƕȡ�
									  4�趨��ת�̶��Ƕȡ�5�¶���ʾ
*******************************************************************************/
void SMG_Display()
{
	int i;
	for(i=0;i<6;i++)
	{
		switch(i)	 //λѡ��ѡ�����������ܣ�
		{
			case(0):
				PA=0xDF; break;//��ʾ��0λ
			case(1):
				PA=0xEF; break;//��ʾ��1λ
			case(2):
				PA=0xF7; break;//��ʾ��2λ
			case(3):
				PA=0xFB; break;//��ʾ��3λ
			case(4):
				PA=0xFD; break;//��ʾ��4λ
			case(5):
				PA=0xFE; break;//��ʾ��5λ
		}
		if(mode%5==0){
			switch(i){
				case(5):if(sj%2==0||st<100)PB=smgduan[direct%2];else PB=0xFF; break;
				case(4):PB=0xFF;break;
				case(3):PB=0xFF;break;
				case(2):PB=0xFF;break;
				case(1):PB=0xFF;break;
				case(0):PB=0xA1;break;
			}
			delay(100); //���һ��ʱ��ɨ��
			PB=0xFF;//����
			if(sj%2==1)
				st = st-1;
			if(st==0){st = 200;}
		}
		if(mode%5==1){
			switch(i){
				case(5):if(sj%2==1&&(st>=100)) PB=0xFF;else PB=smgduan[LV0];break;
				case(4):PB=0xFF;break;
				case(3):PB=0xFF;break;
				case(2):PB=0xFF;break;
				case(1):PB=0xFF;break;
				case(0):PB=0xC1;break;
			}
			delay(100); //���һ��ʱ��ɨ��	
			PB=0xFF;//����
			if(sj%2==1)
				st = st-1;
			if(st==0){st = 200;}
		}
		if(mode%5==2){
			switch(i){
				case(0):if(sj%2==1&&(st>=100)) PB=0xFF; else if(JD/10000==0);else PB=smgduan[JD/10000];break;
				case(1):if(sj%2==1&&(st>=100)) PB=0xFF; else if(JD/1000==0);else PB=smgduan[(JD-JD/10000*10000)/1000];break;
				case(2):if(sj%2==1&&(st>=100)) PB=0xFF; else PB=smgduan[(JD-JD/1000*1000)/100]&0x7F;break;
				case(3):if(sj%2==1&&(st>=100)) PB=0xFF; else PB=smgduan[(JD-JD/100*100)/10];break;
				case(4):if(sj%2==1&&(st>=100)) PB=0xFF; else PB=smgduan[JD%10];break;
				case(5):PB=0x9C;break;
			}
			delay(100); //���һ��ʱ��ɨ��	
			PB=0xFF;//����
			if(sj%2==1)
				st = st-1;
			if(st==0){st = 200;}
		}
		if(mode%5==3){
			switch(i){
				case(0):if(sj%2==1&&(st>=100)&&(wv%5==4)) PB=0xFF; else PB=smgduan[JCC_LED5];break;
				case(1):if(sj%2==1&&(st>=100)&&(wv%5==3)) PB=0xFF; else PB=smgduan[JCC_LED4];break;
				case(2):if(sj%2==1&&(st>=100)&&(wv%5==2)) PB=0xFF; else PB=smgduan[JCC_LED3]&0x7F;break;
				case(3):if(sj%2==1&&(st>=100)&&(wv%5==1)) PB=0xFF; else PB=smgduan[JCC_LED2];break;
				case(4):if(sj%2==1&&(st>=100)&&(wv%5==0)) PB=0xFF; else PB=smgduan[JCC_LED1];break;
				case(5):PB=0x94;break;
			}
			delay(100); //���һ��ʱ��ɨ��	
			PB=0xFF;//����
			if(sj%2==1)
				st = st-1;
			if(st==0){st = 200;}
		}
		if(mode%5==4){
			switch(i){
				case(0):PB=DisplayData[0];break;
				case(1):PB=DisplayData[1];break;
				case(2):PB=DisplayData[2];break;
				case(3):PB=DisplayData[3];break;
				case(4):PB=0x9C;break;
				case(5):PB=0xC6;break;
			}
			delay(100); //���һ��ʱ��ɨ��	
			PB=0xFF;//����
		}
	}
}

/*******************************************************************************
* �� �� ��        : calculate_time
* ��������		   	: ��ʱ�䵵λת����ʱʱ��
*******************************************************************************/
void calculate_time()
{
	switch(V0){
		case(0):TH_ = 0x4C;TL_ = 0x00;break;
		case(1):TH_ = 0x70;TL_ = 0x00;break;
		case(2):TH_ = 0x94;TL_ = 0x00;break;
		case(3):TH_ = 0xB8;TL_ = 0x00;break;
	}
}

/*******************************************************************************
* �� �� ��        : DATA_get
* ��������		   	: ��ȡ����İ���
*******************************************************************************/
void DATA_get()	//������������
{
	if(K1==0)		  //ģʽת��
	{	
		delay(1000);
		if(K1==0)
		{
			mode = mode+1;	//ģʽ�޸�
			sj = 0;					//��ֹ�޸�����
		}
		while(!K1);
	}
	if(K6==0)		  //�Ƿ�ı����
	{	
		delay(1000);
		if(K6==0)
		{
			sj = sj+1;										//���޸Ĳ����벻���޸Ĳ���
		}
		if(sj%2==0&&(mode%5==1))				//ģʽ2���
		{
			V0=LV0;										//�ٶȵ�λ����
			calculate_time();					//�����µĶ�ʱʱ��
		}
		else if(sj%2==0&&(mode%5==0)){	//ģʽ1���
			direct0 = direct;					//�޸ķ���
		}
		else if(sj%2==0&&(mode%5==3)){	//ģʽ4���
			JDC=JCC_LED1+10*JCC_LED2+100*JCC_LED3+1000*JCC_LED4+10000*JCC_LED5;
			TR0=1;										//����ʱ��
			JDC_W = 0;								//�жϱ���
		}
		while(!K6);
	}
	
	if((mode%5)==0&&(sj%2)==1)	//�ı���ת��ת
	{
		if(K4==0)								//����
		{
			delay(1000);
			if(K4==0)
			{
				direct = direct+1;
			}
			while(!K4);
		}
		if(K5==0)								//����
		{
			delay(1000);
			if(K5==0)
			{
				direct = direct+1;
			}
			while(!K5);
		}
	}
	if(mode%5==1&&(sj%2)==1)	//�ı�ת��
	{
		if(K4==0)		//�Ӳ���
		{
			delay(1000);
			if(K4==0)
			{
				LV0=LV0+1;
				if(LV0==4)
					LV0=0;
			}
			while(!K4);
		}
		if(K5==0)		//������
		{
			delay(1000);
			if(K5==0)
			{
				LV0=LV0-1;
				if(LV0==-1)
					LV0=3;
			}
			while(!K5);
		}
	}
	if((mode%5)==2&&(sj%2)==1)	//У���Ƕ�
	{
		if(K4==0)
		{
			delay(1000);
			if(K4==0)
			{
				JD=0;
			}
			while(!K4);
		}
		if(K5==0)
		{
			delay(1000);
			if(K5==0)
			{
				JD=0;
			}
			while(!K5);
		}
	}
	if(mode%5==3&&(sj%2)==1)	//�ı�����
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
				if(wv%5==0){			//�����ÿһλ����
					JCC_LED1+=1;
					if(JCC_LED1==10)
						JCC_LED1=0;
				}
				if(wv%5==1){
					JCC_LED2+=1;
					if(JCC_LED2==10)
						JCC_LED2=0;
				}
				if(wv%5==2){
					JCC_LED3+=1;
					if(JCC_LED3==10)
						JCC_LED3=0;
				}
				if(wv%5==3){
					JCC_LED4+=1;
					if(JCC_LED4==10)
						JCC_LED4=0;
				}
				if(wv%5==4){
					JCC_LED5+=1;
					if(JCC_LED5==10)
						JCC_LED5=0;
				}
			}
			while(!K4);
		}
		if(K5==0)		//������
		{
			delay(1000);
			if(K5==0)
			{
				if(wv%5==0){			//�����ÿһλ����
					JCC_LED1-=1;
					if(JCC_LED1==-1)
						JCC_LED1=9;
				}
				if(wv%5==1){
					JCC_LED2-=1;
					if(JCC_LED2==-1)
						JCC_LED2=9;
				}
				if(wv%5==2){
					JCC_LED3-=1;
					if(JCC_LED3==-1)
						JCC_LED3=9;
				}
				if(wv%5==3){
					JCC_LED4-=1;
					if(JCC_LED4==-1)
						JCC_LED4=9;
				}
				if(wv%5==4){
					JCC_LED5-=1;
					if(JCC_LED5==-1)
						JCC_LED5=9;
				}
			}
			while(!K5);
		}
	}
}

/*******************************************************************************
* �� �� ��        : main
* ��������		   	: ������
*******************************************************************************/
void main()
{	
	DJ_A=1;					//�̶�ס���
	Contadd=0x81;		//�趨8255�Ĺ�����ʽ��PA��PB��Ϊ�����,PC��Ϊ�����
	T0_init();			//���ö�ʱ��0�ж�
	Int0Init();   	//�����ⲿ�ж�0
	TR0=0;					//��ֹ��ʱ��
	EA = 1;					//�����ж�
	while(1){
		DATA_get();											//��ȡ����
		datapros(Ds18b20ReadTemp());	  //�¶����ݴ���
		SMG_Display();									//�������ʾ
	}
}

/*******************************************************************************
* �� �� ��        : Int0()	interrupt 0
* ��������		   	: �ⲿ�ж�0���жϷ������
*******************************************************************************/
void Int0()	interrupt 0
{
	TR0=~TR0;			//�޸ĵ������״̬
	DJ_A=1;				//�����A��λ��ʼ
	DJ_X=0;				//��λ�޸�
}

/*******************************************************************************
* �� �� ��        : Timer0() interrupt 1
* ��������		   	: ��ʱ���ж�0���жϷ������
*******************************************************************************/
void Timer0() interrupt 1	//��ʱ���ж�
{
	if(JDC>0||JDC_W==1){	//����Ԥ���Ƕ�>0���ߵ����ת��־=1���������
		if(direct0%2==0){		//����
			switch(DJ_X){
				case(0):DJ_A=1;DJ_B=0;DJ_C=0;DJ_D=0;break;
				case(1):DJ_A=1;DJ_B=1;DJ_C=0;DJ_D=0;break;
				case(2):DJ_A=0;DJ_B=1;DJ_C=0;DJ_D=0;break;
				case(3):DJ_A=0;DJ_B=1;DJ_C=1;DJ_D=0;break;
				case(4):DJ_A=1;DJ_B=0;DJ_C=1;DJ_D=0;break;
				case(5):DJ_A=0;DJ_B=0;DJ_C=1;DJ_D=1;break;
				case(6):DJ_A=0;DJ_B=0;DJ_C=0;DJ_D=1;break;
				case(7):DJ_A=1;DJ_B=0;DJ_C=0;DJ_D=1;break;
			}
			JD=JD+9;					//���ԽǶ�
			JD=JD%36000;		
		}
		else{								//��ת
			switch(DJ_X){
				case(0):DJ_A=1;DJ_B=0;DJ_C=0;DJ_D=0;break;
				case(1):DJ_A=1;DJ_B=0;DJ_C=0;DJ_D=1;break;
				case(2):DJ_A=0;DJ_B=0;DJ_C=0;DJ_D=1;break;
				case(3):DJ_A=0;DJ_B=0;DJ_C=1;DJ_D=1;break;
				case(4):DJ_A=0;DJ_B=0;DJ_C=1;DJ_D=0;break;
				case(5):DJ_A=0;DJ_B=1;DJ_C=1;DJ_D=0;break;
				case(6):DJ_A=0;DJ_B=1;DJ_C=0;DJ_D=0;break;
				case(7):DJ_A=1;DJ_B=1;DJ_C=0;DJ_D=0;break;
			}
			JD=JD-9;					//���ԽǶ�
			if(JD<=0) JD+=36000;
		}
		DJ_X = (DJ_X + 1)%8;	//��λ�޸�
		if(JDC_W==0){				//���ģʽ4���
			JDC=JDC-9;				//�޸�Ԥ����
			if(JDC<0){				//Ԥ�����ѵ���
				JDC_W=1;				//�޸ı�־
				TR0=0;					//�رն�ʱ�������ͣ�
				DJ_A=1;					//�����λ
			}
		}
	}
	TH0=TH_;	//����ʱ������ֵ����ʱ1ms
	TL0=TL_;
}