org 0000h
sjmp main
org 0040h
main:
	mov sp,#80h
	mov p2,#0ffh
    mov dptr,#0ff23h   ;指向8255的控制字寄存器
	mov a,#81h     ;设置8255的PA、PB为输出，PC为输入
	movx @dptr,a   ;写8255控制字寄存器
xs1:	mov dptr,#0ff21h     ;指向8255的PB口（七段码）
	  	mov a,#0c0h  ;数字0的七段码（共阳）
   		movx @dptr,a ;通过8255的PB口送出七段码
  		mov dptr,#0ff20h   ;位选地址
	  	mov a,#0dfh      ;左边第一个数码管的位选
	   	movx @dptr,a     ;点亮选择左边第一个数码管
  call delay                     ;动态显示的延时
	  	mov a,#0efh      ;左边第二个数码管的位选
	   	movx @dptr,a     ;点亮选择左边第二个数码管
	   call delay
 	  	mov a,#0f7h      ;左边第三个数码管的位选
	   	movx @dptr,a
	    call delay

 	  	mov a,#0fbh      ;左边第四个数码管的位选
	   	movx @dptr,a
     call delay
    	  	mov a,#0fdh	      ;左边第五个数码管的位选
	   	movx @dptr,a
	  call delay

 	  	mov a,#0feh       ;左边第六个数码管的位选
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