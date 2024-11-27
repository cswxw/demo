#ifndef _MYDEBUG_H_
#define _MYDEBUG_H_
#include <stdio.h>
#include <Windows.h>
#define MAX_DBG_MSG_LEN 1024

#define P(...) printf("%s : %s \n", #__VA_ARGS__, __VA_ARGS__);

 



//function
void __dbg_printf(const char * format, ...);

#define INFO(a,...) __dbg_printf("***[%s:%d]*** " ##a "\n",__FUNCTION__,__LINE__,__VA_ARGS__); 
#define ERR(a,...) __dbg_printf("***[%s:%d errcode=%d]*** " ##a "\n",__FUNCTION__,__LINE__,GetLastError(),__VA_ARGS__);
#define SHOW(...) \
	__dbg_printf("***[%s:%d]***",__FUNCTION__,__LINE__); \
	__dbg_printf(__VA_ARGS__);
#endif