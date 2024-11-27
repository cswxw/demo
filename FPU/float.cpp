
vs 2008 :
#include <stdio.h>
int main(int argc , char *argv[]){
00E513A0  push        ebp  
00E513A1  mov         ebp,esp 
00E513A3  sub         esp,0CCh 
00E513A9  push        ebx  
00E513AA  push        esi  
00E513AB  push        edi  
00E513AC  lea         edi,[ebp-0CCh] 
00E513B2  mov         ecx,33h 
00E513B7  mov         eax,0CCCCCCCCh 
00E513BC  rep stos    dword ptr es:[edi] 
	float x=0;
00E513BE  fldz           //把 +0.0 压入压入堆栈中      
00E513C0  fstp        dword ptr [x]    //单精度数据保存到 x，出栈
	x+= 2;
00E513C3  fld         dword ptr [x]    //加载内存中的单精浮点到fpu寄存器堆栈
00E513C6  fadd        qword ptr [__real@4000000000000000 (0E55770h)]   //内从中的32位%st(0)相加
00E513CC  fstp        dword ptr [x]     //单精度数据保存到value，出栈
	x*=10;
00E513CF  fld         dword ptr [x] 
00E513D2  fmul        qword ptr [__real@4024000000000000 (0E55760h)] 
00E513D8  fstp        dword ptr [x] 
	x/=3;
00E513DB  fld         dword ptr [x] 
00E513DE  fdiv        qword ptr [__real@4008000000000000 (0E55750h)] 
00E513E4  fstp        dword ptr [x] 
	x-=5;
00E513E7  fld         dword ptr [x] 
00E513EA  fsub        qword ptr [__real@4014000000000000 (0E55740h)] 
00E513F0  fstp        dword ptr [x] 
	printf("%f",x);
00E513F3  fld         dword ptr [x] 
00E513F6  mov         esi,esp 
00E513F8  sub         esp,8 
00E513FB  fstp        qword ptr [esp] 
00E513FE  push        offset string "%f" (0E5573Ch) 
00E51403  call        dword ptr [__imp__printf (0E582BCh)] 
00E51409  add         esp,0Ch 
00E5140C  cmp         esi,esp 
00E5140E  call        @ILT+310(__RTC_CheckEsp) (0E5113Bh) 
	float x2=3;
00C335D3  fld         dword ptr [__real@40400000 (0C3573Ch)] 
00C335D9  fstp        dword ptr [x2] 

	x*=x2;
00C335DC  fld         dword ptr [x] 
00C335DF  fmul        dword ptr [x2] 
00C335E2  fstp        dword ptr [x] 
	x/=x2;
00C335E5  fld         dword ptr [x] 
00C335E8  fdiv        dword ptr [x2] 
00C335EB  fstp        dword ptr [x] 
	x+=x2;
00C335EE  fld         dword ptr [x] 
00C335F1  fadd        dword ptr [x2] 
00C335F4  fstp        dword ptr [x] 
	x-=x2;
00C335F7  fld         dword ptr [x] 
00C335FA  fsub        dword ptr [x2] 
00C335FD  fstp        dword ptr [x] 

	return 0;
00C33600  xor         eax,eax 
}
00C33602  pop         edi  
00C33603  pop         esi  
00C33604  pop         ebx  
00C33605  add         esp,0D8h 
00C3360B  cmp         ebp,esp 
00C3360D  call        @ILT+310(__RTC_CheckEsp) (0C3113Bh) 
00C33612  mov         esp,ebp 
00C33614  pop         ebp  
00C33615  ret      
