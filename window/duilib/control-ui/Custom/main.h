#ifndef _MAIN_H_
#define _MAIN_H_

#include <windows.h>
#include <objbase.h>
#include <zmouse.h>

#include <UIlib.h>

using namespace DuiLib;

#ifdef _DEBUG
	#ifndef _UNICODE
	#		pragma comment(lib, "DuiLib_d.lib")
	#else
			
	#endif
#else

#endif


#endif //_MAIN_H_ 