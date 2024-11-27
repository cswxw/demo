#ifndef __LOG_TIME_SERVICE_MAIN__
#define __LOG_TIME_SERVICE_MAIN__

#include "../myLib/mylib.h"
#pragma comment(lib, "../Release/mylib.lib")
#include <stdarg.h>
#include <time.h>
#define CONTROL_MOUSE_BUTTONDOWN 128
static TCHAR* myServiceName = TEXT("MyLogTimeService");//·þÎñÃû³Æ  


void LogEvent(LPCTSTR format, ...);
void logTime(char * temp);



#endif // !__LOG_TIME_SERVICE_MAIN__

