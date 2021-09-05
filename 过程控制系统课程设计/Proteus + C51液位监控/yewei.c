#include <reg51.h>
#include <DS18B20.h>
#include <LCD1602.h>
#include <TLC2543.h>

#define	uchar	unsigned char
#define uint	unsigned int

sbit PWM=P2^4;		//���PWM����˿�
sbit SW=P2^5;			//�������ת���ƶ˿�
sbit K1=P3^0;			//�������ð���
sbit K2=P3^1;			//�Ӱ���
sbit K3=P3^2;			//������
sbit temperature_buzzer=P1^4;	//�¶Ⱦ���,40�ȱ���

uchar ratio=10;		//PWMռ�ձ�
uchar count=0;		//PWM��ʱ����
uchar func=0;			//ϵͳ���ܣ�0��ǰ״̬��1���������ã�2���ת�٣�3PID����
uchar high=150,low=50,height=0;
uchar LCD_Buf1[16]={"W = 000.0m"};		//Һλ
uchar LCD_Buf2[16]={"T = 000.0C"};		//�¶�
uchar LCD_Buf3[16]={"H = 150.0m"};		//Һλ����
uchar LCD_Buf4[16]={"L = 050.0m"};		//Һλ����
uchar LCD_Buf5[16]={"Full = 384r/m"};	//ת��
uchar LCD_Buf6[16]={"Now  = 000r/m"};	//Ŀǰת��

int temperature_buzzer_judge=0;	//�ж��Ƿ񱨾���1Ϊ�쳣


//------------------------------------------------------
//��ʾ�¶ȼ�Һλ
//------------------------------------------------------
void Display_HT(void)
{
	uint volt,t;
	//��ѹֵ��ʾ���ݴ���
	volt=TLC2543(0)*50.0/4095*102.3/2.5;
	LCD_Buf1[4]=volt/1000+'0';
	LCD_Buf1[5]=volt/100%10+'0';
	LCD_Buf1[6]=volt/10%10+'0';
	LCD_Buf1[8]=volt%10+'0';
	//�¶�ֵ��ʾ���ݴ���
	t=ReadTemperature();
	LCD_Buf2[4]=t/1000+'0';
	LCD_Buf2[5]=t/100%10+'0';
	LCD_Buf2[6]=t/10%10+'0';
	LCD_Buf2[8]=t%10+'0';
	if(t>400){
		temperature_buzzer=0;	//�¶ȳ���40�ȱ���
		temperature_buzzer_judge+=1;
	}
	else{
		temperature_buzzer=1;	//����
		temperature_buzzer_judge=0;
	}
	LCD_Buf2[15]='A';
	//ˢ��LCD����
	LCD_Display_String(0,0,LCD_Buf1);
	LCD_Display_String(0,1,LCD_Buf2);
}

//------------------------------------------------------
//�������ó���
//------------------------------------------------------
void High_Set()
{
	uint x;
	if(K2==0)				//�Ӱ���
	{	Delay(10);
		if((K2==0)&&(high<200))
			high++;
		while(K2==0);
	}
	if(K3==0)				//������
	{	Delay(10);
		if((K3==0)&&(high>low))
			high--;
		while(K3==0);
	}
	//������ʾ���ݴ���
	x=high*10;
	LCD_Buf3[4]=x/1000+'0';
	LCD_Buf3[5]=x/100%10+'0';
	LCD_Buf3[6]=x/10%10+'0';
	LCD_Buf3[8]=x%10+'0';

	LCD_Buf4[15]='B';
	//ˢ��LCD����
	LCD_Display_String(0,0,LCD_Buf3);
	LCD_Display_String(0,1,LCD_Buf4);
}

//------------------------------------------------------
//�������ó���
//------------------------------------------------------
void Low_Set()
{
	uint x;
	if(K2==0)				//�Ӱ���
	{	Delay(10);
		if((K2==0)&&(low<high))
			low++;
		while(K2==0);
	}
	if(K3==0)				//������
	{	Delay(10);
		if((K3==0)&&(low>1))
			low--;
		while(K3==0);
	}
	//������ʾ���ݴ���
	x=low*10;
	LCD_Buf4[4]=x/1000+'0';
	LCD_Buf4[5]=x/100%10+'0';
	LCD_Buf4[6]=x/10%10+'0';
	LCD_Buf4[8]=x%10+'0';

	LCD_Buf4[15]='C';
	//ˢ��LCD����
	LCD_Display_String(0,0,LCD_Buf3);
	LCD_Display_String(0,1,LCD_Buf4);
}

//------------------------------------------------------
//��ʾ���ȫ���뵱ǰת��
//------------------------------------------------------
void Display_Motor()
{
	uint x;
	//�����ǰת����ʾ���ݴ���
	switch(ratio)
	{
		case 100:	x=384;break;
		case 43:	x=192;break;
		case 26:	x=115;break;
		case 0:		x=0;	
	}
	LCD_Buf6[7]=x/100+'0';
	LCD_Buf6[8]=x/10%10+'0';
	LCD_Buf6[9]=x%10+'0';

	LCD_Buf6[15]='D';
	//ˢ��LCD����
	LCD_Display_String(0,0,LCD_Buf5);
	LCD_Display_String(0,1,LCD_Buf6);

}
//------------------------------------------------------
//K1����ɨ�輰ϵͳ����
//------------------------------------------------------
void Func_Choice()		
{
	if(K1==0)
	{	Delay(10);
		if(K1==0)
			func=(++func)%4;
		while(K1==0);
	}
	switch(func)
	{
		case 0:	Display_HT();break;
		case 1:	High_Set();break;
		case 2: Low_Set();break; 
		case 3: Display_Motor();
	}
}


//------------------------------------------------------
//���ת�ٺ�ת�����
//------------------------------------------------------
void Motor()
{
	uchar l;
	l=TLC2543(0)*5.0/4095*102.3/2.5;
	if(l<low)			//����������ת
	{
		SW=0;
		if(l<low*0.1)	ratio=100;
		if((l>=low*0.1)&&(l<low*0.3))	ratio=42;
		if(l>=low*0.3)	ratio=26;	
	}
	else if(l>high)		//�������޷�ת
	{	
		SW=1;
		if(l<high+(204.7-high)*0.7)		ratio=26;
		if((l>=high+(204.7-high)*0.7)&&(l<high+(204.7-high)*0.9))	ratio=43;
		if(l>=high+(204.7-high)*0.9)	ratio=100;	
	}
	else
		ratio=0;
}


//------------------------------------------------------
//��ʱ�жϳ�ʼ������
//------------------------------------------------------
void T0_Init()
{
	TMOD=0x01;					//����T0������ʽ2
	TH0=(65535-500)/256;		//��ʱ1000us
	TL0=(65536-500)%256;
	EA=1;						//��T0�ж�
	ET0=1;
	TR0=1;						//������ʱ
}
//------------------------------------------------------
//T0�жϷ������
//------------------------------------------------------
void Timer0()	interrupt 1			
{
	TH0=(65535-500)/256;		//��ʱ1000us
	TL0=(65536-500)%256;
	if(ratio==0)	PWM=1;
	else if(ratio==100)	PWM=0;
	else
	{	count++;
		if(count==100)	count=0;
		if(count==1)		PWM=0;
		else if(count==(ratio+1))	PWM=1;
	}
}

//------------------------------------------------------
//�������ݳ�ʼ��
//------------------------------------------------------
void base_init()
{
	temperature_buzzer=1;	//�¶ȱ���ģ������
}

void main()
{
	base_init();	//�������ݳ�ʼ��
	LCD_Init();		//Һ����ʾ����ʼ��
	T0_Init();		//�жϳ�ʼ��
	while(1)
	{
		Func_Choice();		//ϵͳ����ѡ��
		Motor();			//�������
	}
}



