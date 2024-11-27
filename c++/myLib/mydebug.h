#include <Windows.h>
#include <stdio.h>

#define MAX_DBG_MSG_LEN 1024

#define P(...) printf("%s : %s \n", #__VA_ARGS__, __VA_ARGS__);

 



//function
void __dbg_printf(const char * format, ...);