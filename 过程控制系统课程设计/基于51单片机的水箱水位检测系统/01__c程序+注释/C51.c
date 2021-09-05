#include<reg52.h>				        //头文件
#include<intrins.h>
#include"eeprom52.h"			      //STC89C52 EEPROM   程序文件

#define uchar unsigned char		  //宏定义
#define uint unsigned int

#define LCD1602_dat P0	        //LCD1602数据口宏定义

//sbit LCD1602_rs=P2^7;		        //LCD1602控制数据IO口
//sbit LCD1602_rw=P2^4;
//sbit LCD1602_e=P2^6;
sbit LCD1602_rs=P1^0;		        //LCD1602控制数据IO口
sbit LCD1602_rw=P1^1;
sbit LCD1602_e=P2^5;
sbit beep=P1^6;		    					//蜂鸣器  IO
sbit led_1=P3^0;								//LED指示灯  IO
sbit led_2=P3^1;
sbit key_1=P1^5;								//系统控制按键IO口
sbit key_2=P3^3;
sbit key_3=P3^4;
sbit alarm_1=P2^5;						//控制继电器IO口


sbit ADC0832_CS=P1^3;					//ADC0832  控制IO口	  使能口
sbit ADC0832_CLK=P1^0;				//时钟IO口
sbit ADC0832_DIO=P1^1;				//数据输入输出IO口 

uint sum;			  							//10次AD值的综合变量
uchar RH,RH_H=12,RH_L=8,state,ms,cs;  //当前水位，  水位上限，下限，  设置项变量，50ms变量   ，cs 为计次数变量    ，
bit beep1,zt,s1;	 										 //报警标志位，   工作模式标志位，   闪烁标志位

 
unsigned int  A_D()	   							 //ADC0832   读值程序
{
	uchar i;
	uchar dat;					 
	ADC0832_CS=1;     //一个转换周期开始
	ADC0832_CLK=0;    //为第一个脉冲作准备
	ADC0832_CS=0;     //CS置0，片选有效
	ADC0832_DIO=1;    //DIO置1，规定的起始信号  
	ADC0832_CLK=1;    //第一个脉冲
	ADC0832_CLK=0;    //第一个脉冲的下降沿，此前DIO必须是高电平
	ADC0832_DIO=1;    //DIO置1， 通道选择信号  
	ADC0832_CLK=1;    //第二个脉冲，第2、3个脉冲下沉之前，DI必须跟别输入两位数据用于选择通道，这里选通道RH0 
	ADC0832_CLK=0;    //第二个脉冲下降沿 
	ADC0832_DIO=0;    //DI置0，选择通道0
	ADC0832_CLK=1;    //第三个脉冲
	ADC0832_CLK=0;    //第三个脉冲下降沿 
	ADC0832_DIO=1;    //第三个脉冲下沉之后，输入端DIO失去作用，应置1
	ADC0832_CLK=1;    //第四个脉冲
	for(i=0;i<8;i++)  //高位在前
	{
		ADC0832_CLK=1;         //第四个脉冲
		ADC0832_CLK=0; 
		dat<<=1;            //将下面储存的低位数据向右移
		dat|=(unsigned char)ADC0832_DIO; 	 //将输出数据DIO通过或运算储存在dat最低位 
	}	  		        
	ADC0832_CS=1;          //片选无效 
	return dat;	          //将读出的数据返回
}


/********************************************************************
* 名称 : delay()
* 功能 : 小延时。													 
* 输入 : 无
* 输出 : 无
***********************************************************************/
void delay(uint T)					  //延时函数
{
	while(T--);
}
/********************************************************************
* 名称 : LCD1602_write(uchar order,dat)
* 功能 : 1602写如数据函数
* 输入 : 输入的命令值
* 输出 : 无
***********************************************************************/
void LCD1602_write(uchar order,dat)				  //1602 一个字节  处理
{
    LCD1602_e=0;
    LCD1602_rs=order;
    LCD1602_dat=dat;
    LCD1602_rw=0;
    LCD1602_e=1;
    delay(1);
    LCD1602_e=0;																								     
}
/********************************************************************
* 名称 : LCD1602_writebye(uchar *prointer)
* 功能 : 1602写入数据函数  指针式
* 输入 : 输入的命令值
* 输出 : 无
***********************************************************************/
void LCD1602_writebyte(uchar *prointer)				   //1602 字符串    处理
{
    while(*prointer!='\0')
    {
        LCD1602_write(1,*prointer);
        prointer++;
    }
}
/********************************************************************
* 名称 : LCD1602_cls()
* 功能 : 初始化1602液晶 
* 输入 : 无
* 输出 : 无
***********************************************************************/
void LCD1602_cls()									 //1602 初始化
{
	LCD1602_write(0,0x01);     //1602 清屏 指令
	delay(1500);
	LCD1602_write(0,0x38);     // 功能设置 8位、5*7点阵
	delay(1500);
	LCD1602_write(0,0x0c);     //设置 光标   不显示开关、不显示光标、字符不闪烁
	LCD1602_write(0,0x06);
	LCD1602_write(0,0xd0);
	delay(1500);
}
/********************************************************************
* 名称 : show()
* 功能 : LCD1602液晶显示程序 
* 输入 : 无
* 输出 : 无
***********************************************************************/
void show()
{
	if(state==0)		//当前水位及工作模式显示
	{
		
		LCD1602_write(0,0x80);
		LCD1602_writebyte("Water level:");	//当前水位
		LCD1602_write(0,0x80+12);
		if(RH>9)LCD1602_write(1,0x30+RH/10%10);  //十位
		else LCD1602_writebyte(" ");
		LCD1602_write(0,0x80+13);
		LCD1602_write(1,0x30+RH%10);    //个位
		LCD1602_write(0,0x80+14);
		LCD1602_writebyte("cm");

		LCD1602_write(0,0xC0);	
		LCD1602_writebyte("State:");	  //工作模式
		LCD1602_write(0,0xC0+6);
		if(zt==0)
		{
			LCD1602_writebyte("Manul     ");
		}else
		{
			LCD1602_writebyte(" Auto     ");
		}
			
	}
	else		  //水位上下限设置界面
	{
		LCD1602_write(0,0x80);
		LCD1602_writebyte("Water_H:");   //水位上限
		LCD1602_write(0,0x80+8);
		if(state==1&&s1==1)		   //通过闪烁标志为  达到闪烁的效果
		{
			LCD1602_writebyte("  ");
		}else
		{	LCD1602_write(0,0x80+8);
			if(RH_H>9)LCD1602_write(1,0x30+RH_H/10%10);
			else LCD1602_writebyte(" ");
			LCD1602_write(0,0x80+9);
			LCD1602_write(1,0x30+RH_H%10);		
		}
		LCD1602_write(0,0x80+10);
		LCD1602_writebyte("cm    ");

		LCD1602_write(0,0xC0);
		LCD1602_writebyte("Water_L:");  //水位下限
		if(state==2&&s1==1)		 //通过闪烁标志为  达到闪烁的效果
		{
			LCD1602_write(0,0xC0+8);
			LCD1602_writebyte("  "); 
		}else
		{
			LCD1602_write(0,0xC0+8);
			if(RH_L>9)LCD1602_write(1,0x30+RH_L/10%10);
			else LCD1602_writebyte(" ");
			LCD1602_write(0,0xC0+9);
			LCD1602_write(1,0x30+RH_L%10);
		}
		LCD1602_write(0,0xC0+10);
		LCD1602_writebyte("cm    ");	
	}
}
/********************************************************************
* 名称 : key()
* 功能 : 按键控制程序     实现系统各个控制功能 
* 输入 : 无
* 输出 : 无
***********************************************************************/
void key()
{
	if(!key_1) //设置按键  设置  功能：切换显示及设置的选项
	{
		delay(888);   //按键去抖
		if(!key_1)	//再次判断按键
		{
			while(!key_1) show();  //按键判断是否释放
			state=(state+1)%3;	  //执行按键功能   切换设置项
		}
	}

	if(!key_2)   //切换设置项值的大小    及非设置模式下切换系统工作模式按键
	{
		delay(888);   //按键去抖
		if(!key_2)   //再次判断按键
		{
			while(!key_2)show();  //按键判断是否释放
			if(state==1)		  //执行按键功能   切换设置项
			{
				if(RH_H<100)RH_H++;		   //设置上限值
				SectorErase(0x2000);	 //保存上限值	 保存到单片机中EEPROM
				byte_write(0x2000,RH_H);     //字节写入
			}
			else if(state==2)
			{
				if(RH_L<RH_H-1)RH_L++;	  //设置下限值
				SectorErase(0x2200);	 //保存下限值	 保存到单片机中EEPROM
				byte_write(0x2200,RH_L);     //字节写入
			}
			else
			{
				zt=!zt;				 //切换系统的工作模式   自动   手动
				alarm_1=1;   //关闭继电器  
			}	
		}
	}

	if(!key_3)    //切换设置项值的大小    及手动模式下切换系统工作状态‘开关’
	{
		delay(888);	//按键去抖
		if(!key_3)    //再次判断按键
		{		
			while(!key_3)show();  //按键判断是否释放
			if(state==1)		  //执行按键功能   切换设置项
			{
				if(RH_H>RH_L+1)RH_H--;
				SectorErase(0x2000);	 //保存上限值  保存到单片机中EEPROM
				byte_write(0x2000,RH_H);
			}else if(state==2)
			{
				if(RH_L>0)RH_L--;
				SectorErase(0x2200);	 //保存下限值	保存到单片机中EEPROM
				byte_write(0x2200,RH_L);
			}else
			{
				if(zt==0)
				{
					alarm_1=!alarm_1;	   //手动模式切换系统的工作状态
				}
			}
		}
	}		
}
/********************************************************************
* 名称 : proc()
* 功能 : 系统处理程序部分 
* 输入 : 无
* 输出 : 无
***********************************************************************/
void proc()
{
	if(zt==1)	  //zt==1  为自动模式    如果系统在自动模式下
	{
		if(RH>=RH_H)	   //如果当前水位值达到水位上限值则
		{
			alarm_1=1;   //关闭继电器   
			led_1=0;		//显示对应的指示灯
		}
		else		  //否则 
		{
			led_1=1;	   //显示对应的指示灯
		}

		if(RH<=RH_L)	   //如果当前水位值低于水位下限值则
		{
			alarm_1=0;   //开启继电器 
			led_2=0;	   //显示对应的指示灯
		}else	    //否则 
		{
			led_2=1;	   //显示对应的指示灯
		}

		if(RH>=RH_H||RH<=RH_L)  //蜂鸣器处理部分   如果当前水位超出水位上下限  则
		{
			beep1=1;		   //开始报警
		}else		 //否则
		{
			beep1=0;	   //停止报警
		}
	}
	else
	{
		beep1=0;		 //手动模式关闭指示灯及蜂鸣器
		led_1=led_2=1;
	}
}

/********************************************************************
* 名称 : UART_1_init()
* 功能 : 定时器配置初始化
* 输入 : 无
* 输出 : 无
***********************************************************************/
void UART_1_init()
{
	TMOD=0x01;    //定时器配置初始化
	TH0=0x3c;	    //16位定时   定时50ms
	TL0=0xb0;
	ET0=1;
	TR0=1;	   //开启总中断    及打开定时器0
	EA=1;
}



void main()
{	
	float Ad_dat=0;
	alarm_1=1;	    //继电器状态初始化
	UART_1_init();   //定时器配置初始化
	LCD1602_cls();	   //LCD1602  初始化
	RH_H=byte_read(0x2000);	   //读取EEPROM中的水位上下限的值
	RH_L=byte_read(0x2200);
	if((RH_H>99)||(RH_L>99)||(RH_L>=RH_H))   {RH_H=14;  RH_L=7;} //如果超出水位上下限设置的范围，则重新赋值
	   
	while(1)
	{
		if(cs<10)		 		// 滤 //数字滤波器   是ADC0832读出的值变化幅度小
		{
			cs++;
			sum+=A_D();		// 波    //读取10次 AD值
		}
		else					  // 算
		{
			cs=0;				  // 法
			Ad_dat=(uchar)(sum/10);	 //求出当前水位值
			if(Ad_dat>2)
			{
				Ad_dat=(float)((Ad_dat-2)/2.6);  //计算水位
				RH=(uchar)(Ad_dat);	
			}
			else
			RH=0;		
			sum=0;
		}		
		show();	  //调用子程序
		key();
		proc();
	}
}

void UART_1() interrupt 1  //定时器0 中断
{
	TH0=0x3c;			 //重新赋值
	TL0=0xb0;
	ms++;		    //50ms计数
	if(ms%5==0)		//250ms  计时
	{
	    s1=!s1;	    //改变闪烁标志为的值
	}
	if(ms%10==0)	   //500ms定时
	{
		if(beep1==1) //蜂鸣器报警处理   
		{
			beep=!beep;
		}
		else
		{
			beep=1;
		}	
	}
	if(ms>19)
	{
		ms=0;
	}	
}




