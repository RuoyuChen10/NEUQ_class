#include <reg51.h>
#include <DS18B20.h>
#include <LCD1602.h>
#include <TLC2543.h>

#define	uchar	unsigned char
#define uint	unsigned int

sbit PWM=P2^4;		//电机PWM输出端口
sbit SW=P2^5;			//电机正反转控制端口
sbit K1=P3^0;			//功能设置按键
sbit K2=P3^1;			//加按键
sbit K3=P3^2;			//减按键
sbit temperature_buzzer=P1^4;	//温度警报,40度报警

uchar ratio=10;		//PWM占空比
uchar count=0;		//PWM定时计数
uchar func=0;			//系统功能，0当前状态，1上下限设置，2电机转速，3PID参数
uchar high=150,low=50,height=0;
uchar LCD_Buf1[16]={"W = 000.0m"};		//液位
uchar LCD_Buf2[16]={"T = 000.0C"};		//温度
uchar LCD_Buf3[16]={"H = 150.0m"};		//液位上限
uchar LCD_Buf4[16]={"L = 050.0m"};		//液位下限
uchar LCD_Buf5[16]={"Full = 384r/m"};	//转速
uchar LCD_Buf6[16]={"Now  = 000r/m"};	//目前转速

int temperature_buzzer_judge=0;	//判断是否报警，1为异常


//------------------------------------------------------
//显示温度及液位
//------------------------------------------------------
void Display_HT(void)
{
	uint volt,t;
	//电压值显示数据处理
	volt=TLC2543(0)*50.0/4095*102.3/2.5;
	LCD_Buf1[4]=volt/1000+'0';
	LCD_Buf1[5]=volt/100%10+'0';
	LCD_Buf1[6]=volt/10%10+'0';
	LCD_Buf1[8]=volt%10+'0';
	//温度值显示数据处理
	t=ReadTemperature();
	LCD_Buf2[4]=t/1000+'0';
	LCD_Buf2[5]=t/100%10+'0';
	LCD_Buf2[6]=t/10%10+'0';
	LCD_Buf2[8]=t%10+'0';
	if(t>400){
		temperature_buzzer=0;	//温度超过40度报警
		temperature_buzzer_judge+=1;
	}
	else{
		temperature_buzzer=1;	//正常
		temperature_buzzer_judge=0;
	}
	LCD_Buf2[15]='A';
	//刷新LCD数据
	LCD_Display_String(0,0,LCD_Buf1);
	LCD_Display_String(0,1,LCD_Buf2);
}

//------------------------------------------------------
//上限设置程序
//------------------------------------------------------
void High_Set()
{
	uint x;
	if(K2==0)				//加按键
	{	Delay(10);
		if((K2==0)&&(high<200))
			high++;
		while(K2==0);
	}
	if(K3==0)				//减按键
	{	Delay(10);
		if((K3==0)&&(high>low))
			high--;
		while(K3==0);
	}
	//上限显示数据处理
	x=high*10;
	LCD_Buf3[4]=x/1000+'0';
	LCD_Buf3[5]=x/100%10+'0';
	LCD_Buf3[6]=x/10%10+'0';
	LCD_Buf3[8]=x%10+'0';

	LCD_Buf4[15]='B';
	//刷新LCD数据
	LCD_Display_String(0,0,LCD_Buf3);
	LCD_Display_String(0,1,LCD_Buf4);
}

//------------------------------------------------------
//下限设置程序
//------------------------------------------------------
void Low_Set()
{
	uint x;
	if(K2==0)				//加按键
	{	Delay(10);
		if((K2==0)&&(low<high))
			low++;
		while(K2==0);
	}
	if(K3==0)				//减按键
	{	Delay(10);
		if((K3==0)&&(low>1))
			low--;
		while(K3==0);
	}
	//下限显示数据处理
	x=low*10;
	LCD_Buf4[4]=x/1000+'0';
	LCD_Buf4[5]=x/100%10+'0';
	LCD_Buf4[6]=x/10%10+'0';
	LCD_Buf4[8]=x%10+'0';

	LCD_Buf4[15]='C';
	//刷新LCD数据
	LCD_Display_String(0,0,LCD_Buf3);
	LCD_Display_String(0,1,LCD_Buf4);
}

//------------------------------------------------------
//显示电机全速与当前转速
//------------------------------------------------------
void Display_Motor()
{
	uint x;
	//电机当前转速显示数据处理
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
	//刷新LCD数据
	LCD_Display_String(0,0,LCD_Buf5);
	LCD_Display_String(0,1,LCD_Buf6);

}
//------------------------------------------------------
//K1按键扫描及系统功能
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
//电机转速和转向控制
//------------------------------------------------------
void Motor()
{
	uchar l;
	l=TLC2543(0)*5.0/4095*102.3/2.5;
	if(l<low)			//低于下限正转
	{
		SW=0;
		if(l<low*0.1)	ratio=100;
		if((l>=low*0.1)&&(l<low*0.3))	ratio=42;
		if(l>=low*0.3)	ratio=26;	
	}
	else if(l>high)		//高于上限反转
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
//定时中断初始化程序
//------------------------------------------------------
void T0_Init()
{
	TMOD=0x01;					//设置T0工作方式2
	TH0=(65535-500)/256;		//定时1000us
	TL0=(65536-500)%256;
	EA=1;						//开T0中断
	ET0=1;
	TR0=1;						//启动定时
}
//------------------------------------------------------
//T0中断服务程序
//------------------------------------------------------
void Timer0()	interrupt 1			
{
	TH0=(65535-500)/256;		//定时1000us
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
//基本数据初始化
//------------------------------------------------------
void base_init()
{
	temperature_buzzer=1;	//温度报警模块清零
}

void main()
{
	base_init();	//基本数据初始化
	LCD_Init();		//液晶显示器初始化
	T0_Init();		//中断初始化
	while(1)
	{
		Func_Choice();		//系统功能选择
		Motor();			//电机控制
	}
}



