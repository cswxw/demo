gmssl 2.0
	ϵͳ:win10
	vs 2015 x86 ������
	perl
����:
perl Configure VC-WIN32
nmake
nmake install
		
1. ��װnasm  https://www.nasm.us/pub/nasm/releasebuilds/2.13/win32/nasm-2.13-installer-x86.exe
   ��װActivePerl-5.24.3.2404-MSWin32-x64-404865.exe
		http://downloads.activestate.com/ActivePerl/releases/5.24.3.2404/ActivePerl-5.24.3.2404-MSWin32-x64-404865.exe
2. �޸�maskfile 
	engines\skf_dummy.dll: engines\skf_dummy.obj libcrypto.lib engines\e_padlock.obj
	����� engines\e_padlock.obj

