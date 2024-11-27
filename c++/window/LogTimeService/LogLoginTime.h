#ifndef _LOG_LOGIN_TIME_
#define _LOG_LOGIN_TIME_
#include "../myLib/mylib.h"
#pragma comment(lib, "../Debug/mylib.lib")


#include <Wtsapi32.h>
#pragma comment(lib,"Wtsapi32.lib")


LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
DWORD WINAPI CreateLogLoginTime(LPVOID lpParam);
#endif //_LOG_LOGIN_TIME_