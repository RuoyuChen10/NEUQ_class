/* ���ƣ���ֻ�����ѭ����ʾ0~9
˵�����������е�ѭ����䷴����0~9 �Ķ�������PB �ڣ�ʹ����0~9 ѭ����ʾ
*/
#include <reg52.h>
#include <absacc.h>
#include<intrins.h>
#define Contadd XBYTE[0xff23] 		//8255���Ƶ�ַ
#define PA XBYTE[0xff20]        	//��ѡ�źŵ�ַ
#define PB XBYTE[0xff21] 	 	//Ƭѡ�źŵ�ַ 
#define adc XBYTE [0xFF80] 		//Ƭѡָ�룬A/Dת����·Ƭ���ַ
#define uchar unsigned char
#define uint unsigned int
uchar code DSY_CODE[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e,0xff};//����������ܶ���
//��ʱ
void DelayMS(uint x)
{
uchar t;
while(x--) for(t=0;t<120;t++);
}
//������
void main()
{
uchar i=0;
Contadd=0x81; //�趨8255�Ĺ�����ʽ��PA��PB��Ϊ�����,PC��Ϊ�����
PA=0xdf;      //pa,ѡ�����һ�������
PB=0xff;
while(1)
{
PB=DSY_CODE[i];
i=(i+1)%10;
DelayMS(300);
}
}