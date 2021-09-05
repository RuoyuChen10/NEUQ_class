/* 名称：单只数码管循环显示0~9
说明：主程序中的循环语句反复将0~9 的段码送至PB 口，使数字0~9 循环显示
*/
#include <reg52.h>
#include <absacc.h>
#include<intrins.h>
#define Contadd XBYTE[0xff23] 		//8255控制地址
#define PA XBYTE[0xff20]        	//段选信号地址
#define PB XBYTE[0xff21] 	 	//片选信号地址 
#define adc XBYTE [0xFF80] 		//片选指针，A/D转换电路片外地址
#define uchar unsigned char
#define uint unsigned int
uchar code DSY_CODE[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e,0xff};//共阳极数码管段码
//延时
void DelayMS(uint x)
{
uchar t;
while(x--) for(t=0;t<120;t++);
}
//主程序
void main()
{
uchar i=0;
Contadd=0x81; //设定8255的工作方式：PA、PB口为输出口,PC口为输入口
PA=0xdf;      //pa,选中最后一个数码管
PB=0xff;
while(1)
{
PB=DSY_CODE[i];
i=(i+1)%10;
DelayMS(300);
}
}