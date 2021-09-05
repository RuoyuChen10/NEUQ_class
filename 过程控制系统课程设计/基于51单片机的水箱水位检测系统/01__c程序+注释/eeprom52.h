#ifndef _EEPROM52_H_
#define _EEPROM52_H_
#include <intrins.h>

/********STC89C51扇区分布*******
第一扇区：1000H--11FF
第二扇区：1200H--13FF
第三扇区：1400H--15FF
第四扇区：1600H--17FF
第五扇区：1800H--19FF
第六扇区：1A00H--1BFF
第七扇区：1C00H--1DFF
第八扇区：1E00H--1FFF
*****************/

/********STC89C52扇区分布*******
第一扇区：2000H--21FF
第二扇区：2200H--23FF
第三扇区：2400H--25FF
第四扇区：2600H--27FF
第五扇区：2800H--29FF
第六扇区：2A00H--2BFF
第七扇区：2C00H--2DFF
第八扇区：2E00H--2FFF
*****************/


#define RdCommand 0x01 //定义ISP的操作命令
#define PrgCommand 0x02
#define EraseCommand 0x03 
#define Error 1
#define Ok 0
#define WaitTime 0x01 //定义CPU的等待时间
sfr ISP_DATA=0xe2;  //寄存器申明
sfr ISP_ADDRH=0xe3;
sfr ISP_ADDRL=0xe4;
sfr ISP_CMD=0xe5;
sfr ISP_TRIG=0xe6;
sfr ISP_CONTR=0xe7;

/* ================ 打开 ISP,IAP 功能 ================= */
void ISP_IAP_enable(void)
{
	 EA = 0;       /* 关中断   */
	 ISP_CONTR = ISP_CONTR & 0x18;       /* 0001,1000 */
	 ISP_CONTR = ISP_CONTR | WaitTime; /* 写入硬件延时 */
	 ISP_CONTR = ISP_CONTR | 0x80;       /* ISPEN=1  */
}
/* =============== 关闭 ISP,IAP 功能 ================== */
void ISP_IAP_disable(void)
{
	 ISP_CONTR = ISP_CONTR & 0x7f; /* ISPEN = 0 */
	 ISP_TRIG = 0x00;
	 EA   =   1;   /* 开中断 */
}
/* ================ 公用的触发代码 ==================== */
void ISPgoon(void)
{
	 ISP_IAP_enable();   /* 打开 ISP,IAP 功能 */
	 ISP_TRIG = 0x46;  /* 触发ISP_IAP命令字节1 */
	 ISP_TRIG = 0xb9;  /* 触发ISP_IAP命令字节2 */
	 _nop_();
}
/* ==================== 字节读 ======================== */
unsigned char byte_read(unsigned int byte_addr)
{
	EA = 0;
	 ISP_ADDRH = (unsigned char)(byte_addr >> 8);/* 地址赋值 */
	 ISP_ADDRL = (unsigned char)(byte_addr & 0x00ff);
	 ISP_CMD   = ISP_CMD & 0xf8;   /* 清除低3位  */
	 ISP_CMD   = ISP_CMD | RdCommand; /* 写入读命令 */
	 ISPgoon();       /* 触发执行  */
	 ISP_IAP_disable();    /* 关闭ISP,IAP功能 */
	 EA  = 1;
	 return (ISP_DATA);    /* 返回读到的数据 */
}
/* ================== 扇区擦除 ======================== */
void SectorErase(unsigned int sector_addr)
{
	 unsigned int iSectorAddr;
	 iSectorAddr = (sector_addr & 0xfe00); /* 取扇区地址 */
	 ISP_ADDRH = (unsigned char)(iSectorAddr >> 8);
	 ISP_ADDRL = 0x00;
	 ISP_CMD = ISP_CMD & 0xf8;   /* 清空低3位  */
	 ISP_CMD = ISP_CMD | EraseCommand; /* 擦除命令3  */
	 ISPgoon();       /* 触发执行  */
	 ISP_IAP_disable();    /* 关闭ISP,IAP功能 */
}
/* ==================== 字节写 ======================== */
void byte_write(unsigned int byte_addr, unsigned char original_data)
{
	 EA  = 0;
//	 SectorErase(byte_addr);
	 ISP_ADDRH = (unsigned char)(byte_addr >> 8);  /* 取地址  */
	 ISP_ADDRL = (unsigned char)(byte_addr & 0x00ff);
	 ISP_CMD  = ISP_CMD & 0xf8;    /* 清低3位 */
	 ISP_CMD  = ISP_CMD | PrgCommand;  /* 写命令2 */
	 ISP_DATA = original_data;   /* 写入数据准备 */
	 ISPgoon();       /* 触发执行  */
	 ISP_IAP_disable();     /* 关闭IAP功能 */
	 EA =1;
}
		
#endif