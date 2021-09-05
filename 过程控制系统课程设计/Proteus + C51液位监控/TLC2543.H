#include <reg51.h>
sbit SDO=P1^0;		//TLC2543数据输入端
sbit SDI=P1^1;		//TLC2543数据输出端
sbit CS=P1^2;		//TLC2543片选端
sbit CLK=P1^3;		//TLC2543时钟

#define	uchar	unsigned char
#define uint	unsigned int
//------------------------------------------------------
//延时函数（n ms）
//------------------------------------------------------
void Delay_AD(uint n)
{
	uchar i;
	uint j;
	for(j=0;j<n;j++)
		for(i=0;i<120;i++);
}

//------------------------------------------------------
//TLC2543 AD转换程序
//port:转换通道号，0~10
//------------------------------------------------------
uint TLC2543(uchar port)
{
	uint i;
	uint ad_value=0;
	CLK=0;
	CS=0;
	port<<=4;
	//读取12位串行转换结果
	for(i=0;i<12;i++)
	{ 	
		if(SDO) ad_value|=0x01;
		SDI=(bit)(port&0x80);
		CLK=1;
		Delay_AD(1);
		CLK=0;
		Delay_AD(1);
		port=port<<1;
		ad_value=ad_value<<1;
	}
	CS=1;
	ad_value=ad_value>>1;
	return ad_value;
}