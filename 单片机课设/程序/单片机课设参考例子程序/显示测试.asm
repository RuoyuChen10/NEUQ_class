org 0000h
sjmp main
org 0040h
main:
	mov sp,#80h
	mov p2,#0ffh
    mov dptr,#0ff23h   ;ָ��8255�Ŀ����ּĴ���
	mov a,#81h     ;����8255��PA��PBΪ�����PCΪ����
	movx @dptr,a   ;д8255�����ּĴ���
xs1:	mov dptr,#0ff21h     ;ָ��8255��PB�ڣ��߶��룩
	  	mov a,#0c0h  ;����0���߶��루������
   		movx @dptr,a ;ͨ��8255��PB���ͳ��߶���
  		mov dptr,#0ff20h   ;λѡ��ַ
	  	mov a,#0dfh      ;��ߵ�һ������ܵ�λѡ
	   	movx @dptr,a     ;����ѡ����ߵ�һ�������
  call delay                     ;��̬��ʾ����ʱ
	  	mov a,#0efh      ;��ߵڶ�������ܵ�λѡ
	   	movx @dptr,a     ;����ѡ����ߵڶ��������
	   call delay
 	  	mov a,#0f7h      ;��ߵ���������ܵ�λѡ
	   	movx @dptr,a
	    call delay

 	  	mov a,#0fbh      ;��ߵ��ĸ�����ܵ�λѡ
	   	movx @dptr,a
     call delay
    	  	mov a,#0fdh	      ;��ߵ��������ܵ�λѡ
	   	movx @dptr,a
	  call delay

 	  	mov a,#0feh       ;��ߵ���������ܵ�λѡ
	   	movx @dptr,a
		  call delay
	sjmp xs1
delay:
         mov r1,#10        ;20ms
         d2:mov r2,#120	  
         djnz r2,$
         djnz r1,d2
         ret
	end