
#include "MyDebug.h"

#ifdef _DEBUG
void __dbg_printf(const char * format, ...)
{
	char buf[MAX_DBG_MSG_LEN];
	va_list ap;
	va_start(ap, format);
	vsprintf(buf, format, ap);
	va_end(ap);
	printf(buf);
}
#else   
 void __dbg_printf(const char * format, ...) {}
#endif 
