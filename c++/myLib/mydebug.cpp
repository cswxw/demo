#include "mydebug.h" 

#ifdef _DEBUG
void __dbg_printf(const TCHAR * format, ...)
{
#ifdef  UNICODE 
	OutputDebugString(format);
#else
	char buf[MAX_DBG_MSG_LEN];
	va_list ap;
	va_start(ap, format);
	_vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);
	OutputDebugString(buf);
#endif
}
#else   
static void __dbg_printf(const char * format, ...) {}
#endif 