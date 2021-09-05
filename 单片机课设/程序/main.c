#include <reg52.h>
#include <absacc.h>
#include<intrins.h>
#include"temp.h"
#define Contadd XBYTE[0xff23] 		//8255¿ØÖÆµØÖ·
#define PA XBYTE[0xff20]        	//¶ÎÑ¡ĞÅºÅµØÖ·
#define PB XBYTE[0xff21] 	 	//Æ¬Ñ¡ĞÅºÅµØÖ· 
#define adc XBYTE [0xFF80] 		//Æ¬Ñ¡Ö¸Õë£¬A/D×ª»»µçÂ·Æ¬ÍâµØÖ·
#define uchar unsigned char		//ºê¶¨Òå	
#define uint unsigned int
typedef unsigned int u16;	  //¶ÔÊı¾İÀàĞÍ½øĞĞÉùÃ÷¶¨Òå
typedef unsigned char u8;	

sbit DJ_A=P1^0;		//µç»úµÄ4¸öÏàÎ»
sbit DJ_B=P1^1;
sbit DJ_C=P1^2;
sbit DJ_D=P1^3;

int DJ_X = 0;		//µç»úÏà½ÇÅĞ¶Ï
long int JD = 0;	//µç»ú½Ç¶È

sbit RUN=P3^2;		//ÆôÍ£¿ª¹Ø
sbit K1=P1^4;			//Ä£Ê½Ñ¡Ôñ
sbit K2=P1^5;			//×óÒÆ
sbit K3=P1^6;			//ÓÒÒÆ
sbit K4=P1^7;			//¼Ó
sbit K5=P3^0;			//¼õ
sbit K6=P3^1;			//È·¶¨

int LV0 = 0;			//ÊıÂë¹ÜÏÔÊ¾ËÙ¶È
int V0 = 0;				//ËÙ¶È

int st=200;				//ÊıÂë¹ÜÉÁË¸
int sj = 0;				//ÊÇ·ñµ÷Êı

int direct=0;			//×ªÏòÅĞ¶Ï
int direct0=0;
int mode = 0;			//Ä£Ê½
int go = 0;				//ÊÇ·ñÆô¶¯

long int JDC=0;
int JDC_W = 1;		//½Ç¶ÈĞı×ª±êÖÂ
int JCC_LED1=0;		//µç»ú½Ç¶ÈLEDÏÔÊ¾
int JCC_LED2=0;
int JCC_LED3=0;
int JCC_LED4=0;
int JCC_LED5=0;
int wv=60;				//Ñ­»·Î»

char num=0;
unsigned char DisplayData[4];	//ÎÂ¶ÈÊı¾İ
int T_judge=0;								//ÎÂ¶ÈÂË²¨

int TH_ = 0x4C;		//ÉèÖÃ³õÂö¿í0.02s
int TL_ = 0x00;

unsigned char code smgduan[17]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,
				0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};//ÏÔÊ¾0~FµÄÖµ


/*******************************************************************************
* º¯ Êı Ãû 				: delay
* º¯Êı¹¦ÄÜ				: ÑÓÊ±º¯Êı£¬i=1Ê±£¬´óÔ¼ÑÓÊ±10us
*******************************************************************************/
void delay(unsigned int i)
{
	while(i--);	
}

/*******************************************************************************
* º¯ Êı Ãû     		: T0_init
* º¯Êı¹¦ÄÜ				: ¶¨Ê±Æ÷T0³õÊ¼»¯£¬³õ¶¨¼ÆÊ±0.02s
*******************************************************************************/
void T0_init(void)
{
	TMOD|=0X0;	//¹¤×÷·½Ê½1£¬¼ÆÊ±Ê¹ÓÃ
	TH0=TH_;	//³õÖµ
	TL0=TL_;	
	ET0=1;
}

/*******************************************************************************
* º¯ Êı Ãû        : Int0Init
* º¯Êı¹¦ÄÜ		   	: Íâ²¿ÖĞ¶ÏINT0³õÊ¼»¯£¬ÏÂ½µÑØ´¥·¢
*******************************************************************************/
void Int0Init()
{
	//ÉèÖÃINT0
	IT0=1;//Ìø±äÑØ³ö·¢·½Ê½£¨ÏÂ½µÑØ£©
	EX0=1;//´ò¿ªINT0µÄÖĞ¶ÏÔÊĞí¡£	
}

/*******************************************************************************
* º¯ Êı Ãû        : datapros
* º¯Êı¹¦ÄÜ		   	: ÎÂ¶È¶ÁÈ¡×ª»»º¯Êı
* ÊäÈë						: ÎÂ¶ÈÊıÖµtemp
*******************************************************************************/
void datapros(int temp) 	 
{
   	float tp;  
	if(temp< 0)				//µ±ÎÂ¶ÈÖµÎª¸ºÊı
  	{
		DisplayData[0] = 0xBF; 	  //   -
		//ÒòÎª¶ÁÈ¡µÄÎÂ¶ÈÊÇÊµ¼ÊÎÂ¶ÈµÄ²¹Âë£¬ËùÒÔ¼õ1£¬ÔÙÈ¡·´Çó³öÔ­Âë
		temp=temp-1;
		temp=~temp;
		tp=temp;
		temp=tp*0.0625*100+0.5;	
		//ÁôÁ½¸öĞ¡Êıµã¾Í*100£¬+0.5ÊÇËÄÉáÎåÈë£¬ÒòÎªCÓïÑÔ¸¡µãÊı×ª»»ÎªÕûĞÍµÄÊ±ºò°ÑĞ¡Êıµã
		//ºóÃæµÄÊı×Ô¶¯È¥µô£¬²»¹ÜÊÇ·ñ´óÓÚ0.5£¬¶ø+0.5Ö®ºó´óÓÚ0.5µÄ¾ÍÊÇ½ø1ÁË£¬Ğ¡ÓÚ0.5µÄ¾Í
		//Ëã¼ÓÉÏ0.5£¬»¹ÊÇÔÚĞ¡ÊıµãºóÃæ¡£
 
  	}
 	else
  	{			
		DisplayData[0] = 0xFF;
		tp=temp;//ÒòÎªÊı¾İ´¦ÀíÓĞĞ¡ÊıµãËùÒÔ½«ÎÂ¶È¸³¸øÒ»¸ö¸¡µãĞÍ±äÁ¿
		//Èç¹ûÎÂ¶ÈÊÇÕıµÄÄÇÃ´£¬ÄÇÃ´ÕıÊıµÄÔ­Âë¾ÍÊÇ²¹ÂëËü±¾Éí
		temp=tp*0.0625*100+0.5;	
		//ÁôÁ½¸öĞ¡Êıµã¾Í*100£¬+0.5ÊÇËÄÉáÎåÈë£¬ÒòÎªCÓïÑÔ¸¡µãÊı×ª»»ÎªÕûĞÍµÄÊ±ºò°ÑĞ¡Êıµã
		//ºóÃæµÄÊı×Ô¶¯È¥µô£¬²»¹ÜÊÇ·ñ´óÓÚ0.5£¬¶ø+0.5Ö®ºó´óÓÚ0.5µÄ¾ÍÊÇ½ø1ÁË£¬Ğ¡ÓÚ0.5µÄ¾Í
		//Ëã¼ÓÉÏ0.5£¬»¹ÊÇÔÚĞ¡ÊıµãºóÃæ¡£
	}
	DisplayData[0] = smgduan[temp / 10000];
	DisplayData[1] = smgduan[temp % 10000 / 1000];
	DisplayData[2] = smgduan[temp % 1000 / 100] & 0x7F;
	DisplayData[3] = smgduan[temp % 100 / 10];
	if(temp/10>=315){				//¸ßÎÂ±¨¾¯
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
* º¯ Êı Ãû        : SMG_Display
* º¯Êı¹¦ÄÜ		   	: ÊıÂë¹ÜÏÔÊ¾º¯Êı£¬ÒÀ´ÎÏÔÊ¾5ÖÖÄ£Ê½£¬¼´1·½Ïò¡¢2ËÙ¶ÈµµÎ»¡¢3½Ç¶È¡¢
									  4Éè¶¨Ğı×ª¹Ì¶¨½Ç¶È¡¢5ÎÂ¶ÈÏÔÊ¾
*******************************************************************************/
void SMG_Display()
{
	int i;
	for(i=0;i<6;i++)
	{
		switch(i)	 //Î»Ñ¡£¬Ñ¡ÔñµãÁÁµÄÊıÂë¹Ü£¬
		{
			case(0):
				PA=0xDF; break;//ÏÔÊ¾µÚ0Î»
			case(1):
				PA=0xEF; break;//ÏÔÊ¾µÚ1Î»
			case(2):
				PA=0xF7; break;//ÏÔÊ¾µÚ2Î»
			case(3):
				PA=0xFB; break;//ÏÔÊ¾µÚ3Î»
			case(4):
				PA=0xFD; break;//ÏÔÊ¾µÚ4Î»
			case(5):
				PA=0xFE; break;//ÏÔÊ¾µÚ5Î»
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
			delay(100); //¼ä¸ôÒ»¶ÎÊ±¼äÉ¨Ãè
			PB=0xFF;//ÏûÒş
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
			delay(100); //¼ä¸ôÒ»¶ÎÊ±¼äÉ¨Ãè	
			PB=0xFF;//ÏûÒş
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
			delay(100); //¼ä¸ôÒ»¶ÎÊ±¼äÉ¨Ãè	
			PB=0xFF;//ÏûÒş
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
			delay(100); //¼ä¸ôÒ»¶ÎÊ±¼äÉ¨Ãè	
			PB=0xFF;//ÏûÒş
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
			delay(100); //¼ä¸ôÒ»¶ÎÊ±¼äÉ¨Ãè	
			PB=0xFF;//ÏûÒş
		}
	}
}

/*******************************************************************************
* º¯ Êı Ãû        : calculate_time
* º¯Êı¹¦ÄÜ		   	: ÓÉÊ±¼äµµÎ»×ª»»¶¨Ê±Ê±¼ä
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
* º¯ Êı Ãû        : DATA_get
* º¯Êı¹¦ÄÜ		   	: »òÈ¡ÊäÈëµÄ°´¼ü
*******************************************************************************/
void DATA_get()	//°´¼üÊäÈëÊı¾İ
{
	if(K1==0)		  //Ä£Ê½×ª»»
	{	
		delay(1000);
		if(K1==0)
		{
			mode = mode+1;	//Ä£Ê½ĞŞ¸Ä
			sj = 0;					//½ûÖ¹ĞŞ¸ÄÊı×Ö
		}
		while(!K1);
	}
	if(K6==0)		  //ÊÇ·ñ¸Ä±ä²ÎÊı
	{	
		delay(1000);
		if(K6==0)
		{
			sj = sj+1;										//¿ÉĞŞ¸Ä²ÎÊıÓë²»¿ÉĞŞ¸Ä²ÎÊı
		}
		if(sj%2==0&&(mode%5==1))				//Ä£Ê½2Çé¿ö
		{
			V0=LV0;										//ËÙ¶ÈµµÎ»¸üĞÂ
			calculate_time();					//¼ÆËãĞÂµÄ¶¨Ê±Ê±¼ä
		}
		else if(sj%2==0&&(mode%5==0)){	//Ä£Ê½1Çé¿ö
			direct0 = direct;					//ĞŞ¸Ä·½Ïò
		}
		else if(sj%2==0&&(mode%5==3)){	//Ä£Ê½4Çé¿ö
			JDC=JCC_LED1+10*JCC_LED2+100*JCC_LED3+1000*JCC_LED4+10000*JCC_LED5;
			TR0=1;										//¿ª¶¨Ê±Æ÷
			JDC_W = 0;								//ÅĞ¶Ï±êÖÂ
		}
		while(!K6);
	}
	
	if((mode%5)==0&&(sj%2)==1)	//¸Ä±äÕı×ª·´×ª
	{
		if(K4==0)								//Ôö¼Ó
		{
			delay(1000);
			if(K4==0)
			{
				direct = direct+1;
			}
			while(!K4);
		}
		if(K5==0)								//¼õÉÙ
		{
			delay(1000);
			if(K5==0)
			{
				direct = direct+1;
			}
			while(!K5);
		}
	}
	if(mode%5==1&&(sj%2)==1)	//¸Ä±ä×ªËÙ
	{
		if(K4==0)		//¼Ó²Ù×÷
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
		if(K5==0)		//¼õ²Ù×÷
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
	if((mode%5)==2&&(sj%2)==1)	//Ğ£Õı½Ç¶È
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
	if(mode%5==3&&(sj%2)==1)	//¸Ä±ä³õÉè½Ç
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
		if(K4==0)		//¼Ó²Ù×÷
		{
			delay(1000);
			if(K4==0)
			{
				if(wv%5==0){			//ÊıÂë¹ÜÃ¿Ò»Î»²Ù×÷
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
		if(K5==0)		//¼õ²Ù×÷
		{
			delay(1000);
			if(K5==0)
			{
				if(wv%5==0){			//ÊıÂë¹ÜÃ¿Ò»Î»²Ù×÷
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
* º¯ Êı Ãû        : main
* º¯Êı¹¦ÄÜ		   	: Ö÷º¯Êı
*******************************************************************************/
void main()
{	
	DJ_A=1;					//¹Ì¶¨×¡µç»ú
	Contadd=0x81;		//Éè¶¨8255µÄ¹¤×÷·½Ê½£ºPA¡¢PB¿ÚÎªÊä³ö¿Ú,PC¿ÚÎªÊäÈë¿Ú
	T0_init();			//ÉèÖÃ¶¨Ê±Æ÷0ÖĞ¶Ï
	Int0Init();   	//ÉèÖÃÍâ²¿ÖĞ¶Ï0
	TR0=0;					//½ûÖ¹¶¨Ê±Æ÷
	EA = 1;					//¿ª×ÜÖĞ¶Ï
	while(1){
		DATA_get();											//»ñÈ¡°´¼ü
		datapros(Ds18b20ReadTemp());	  //ÎÂ¶ÈÊı¾İ´¦Àí
		SMG_Display();									//ÊıÂë¹ÜÏÔÊ¾
	}
}

/*******************************************************************************
* º¯ Êı Ãû        : Int0()	interrupt 0
* º¯Êı¹¦ÄÜ		   	: Íâ²¿ÖĞ¶Ï0µÄÖĞ¶Ï·şÎñ³ÌĞò
*******************************************************************************/
void Int0()	interrupt 0
{
	TR0=~TR0;			//ĞŞ¸Äµç»ú¹¤×÷×´Ì¬
	DJ_A=1;				//µç»ú´ÓAÏàÎ»¿ªÊ¼
	DJ_X=0;				//ÏàÎ»ĞŞ¸Ä
}

/*******************************************************************************
* º¯ Êı Ãû        : Timer0() interrupt 1
* º¯Êı¹¦ÄÜ		   	: ¶¨Ê±Æ÷ÖĞ¶Ï0µÄÖĞ¶Ï·şÎñ³ÌĞò
*******************************************************************************/
void Timer0() interrupt 1	//¶¨Ê±Æ÷ÖĞ¶Ï
{
	if(JDC>0||JDC_W==1){	//Âú×ãÔ¤¶¨½Ç¶È>0»òÕßµç»úĞı×ª±êÖ¾=1¿ÉÆô¶¯µç»ú
		if(direct0%2==0){		//Õı´«
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
			JD=JD+9;					//¾ø¶Ô½Ç¶È
			JD=JD%36000;		
		}
		else{								//·´×ª
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
			JD=JD-9;					//¾ø¶Ô½Ç¶È
			if(JD<=0) JD+=36000;
		}
		DJ_X = (DJ_X + 1)%8;	//ÏàÎ»ĞŞ¸Ä
		if(JDC_W==0){				//Èç¹ûÄ£Ê½4Çé¿ö
			JDC=JDC-9;				//ĞŞ¸ÄÔ¤¶¨½Ç
			if(JDC<0){				//Ô¤¶¨½ÇÒÑµ½´ï
				JDC_W=1;				//ĞŞ¸Ä±êÖ¾
				TR0=0;					//¹Ø±Õ¶¨Ê±Æ÷£¬µç»úÍ£Ö
				DJ_A=1;					//µç»úÏàÎ»
			}
		}
	}
	TH0=TH_;	//¸ø¶¨Ê±Æ÷¸³³õÖµ£¬¶¨Ê±1ms
	TL0=TL_;
}