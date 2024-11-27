#ifndef log_info
#include <syslog.h>
#define log_info(fmt, ...)  \
     do{\
     openlog("lightdm-webkit", LOG_PID, LOG_DAEMON); \
	 	 syslog(LOG_INFO, "%s(%d)-[%s] " fmt,__FILE__,__LINE__,__FUNCTION__, ##__VA_ARGS__); \
     closelog();\
     }while(0);
#endif

