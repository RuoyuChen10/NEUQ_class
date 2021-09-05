#include <reg51.h>
#define	uchar	unsigned char
#define uint	unsigned int

#define DP  P0				//LCD数据I/O口
sbit RS=P2^0;		//LCD寄存器选择
sbit RW=P2^1;		//LCD读/写控制
sbit EN=P2^2;		//LCD使能端

//------------------------------------------------------
//延时函数（n ms）
//------------------------------------------------------
void Delay(uint n)
{
	uchar i;
	uint j;
	for(j=0;j<n;j++)
		for(i=0;i<120;i++);
}

//------------------------------------------------------
//LCD1602忙检查
//------------------------------------------------------
uchar LCD_Busy_Check()
{
	uchar LCD_Status;
	RS=0;
	RW=1;
	EN=1;
	Delay(1);
	LCD_Status=DP;
	EN=0;
	return LCD_Status;
}


//------------------------------------------------------
//LCD1602写命令函数
//------------------------------------------------------
void LCD_Write_Cmd(uchar cmd)
{
	while((LCD_Busy_Check()&0x80)==0x80);	//忙等待
	RS=0;	//选择命令寄存器
	RW=0;	//写
	EN=0;	
	DP=cmd;
	EN=1;
	Delay(1);
	EN=0;
}

//------------------------------------------------------
//LCD1602写数据函数
//------------------------------------------------------
void LCD_Write_Data(uchar dat)
{
	while((LCD_Busy_Check()&0x80)==0x80);	//忙等待
	RS=1;
	EN=0;
	RW=0;
	DP=dat;
	EN=1;
	Delay(1);
	EN=0;
}

//------------------------------------------------------
//LCD1602初始化函数
//------------------------------------------------------
void LCD_Init()
{
	LCD_Write_Cmd(0x38);		//三次显示模式设置，不检测忙信号
	Delay(5);
	LCD_Write_Cmd(0x38);
	Delay(5);
	LCD_Write_Cmd(0x38);
	Delay(5);

	LCD_Write_Cmd(0x38);		//显示模式设置，开始要求检测忙信号：8位、2行、5X7点阵
	Delay(5);
	LCD_Write_Cmd(0x01);		//清屏
	Delay(5);
	LCD_Write_Cmd(0x06);		//字符进入模式:屏幕不动，字符后移
	Delay(5);
	LCD_Write_Cmd(0x0C);		//显示开，光标关
	Delay(5);
}

//------------------------------------------------------
//LCD1602显示一行字符串函数
//x:显示起始列号,0~15
//y:显示起始行号,0:第一行,1:第二行
//*str：字符串指针
//------------------------------------------------------
void LCD_Display_String(uchar x,uchar y,uchar *str)		
{
	uchar i=0;
	//设置显示起始位置
	if(y==0)	LCD_Write_Cmd(0x80|x);		
	if(y==1)	LCD_Write_Cmd(0xC0|x);
	//显示字符串
	for(i=0;i<16;i++)
	{
		LCD_Write_Data(str[i]);
		Delay(1);
	}
}