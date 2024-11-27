#ifndef _MY_LIB_
#define _MY_LIB_
#include <winsock2.h>
#pragma comment(lib, "WS2_32")	// Á´½Óµ½WS2_32.lib
#include <stdlib.h>
#include <Windows.h>
#include "mydebug.h"

#define LIB_ERROR -1;
#define LIB_OK 0





int getFirstIP(char *buf, int size);
BOOL WINAPI ControlServiceCustomCode(LPCTSTR ServiceName, DWORD dwControl);
#endif //_MY_LIB_