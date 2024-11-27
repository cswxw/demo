#ifndef __LOG_H_H__
#define __LOG_H_H__

#define		LOG_INFO	0
#define     LOG_WARNING 1
#define		LOG_ERROR	2


#if defined(_WIN32) || defined(WIN32)
	#define 	LOG_DIR				"d:/"			
#else
	#define 	LOG_DIR				"/tmp/"			
#endif


void LogString(unsigned long logType, char* fmt, ...);
void LogData(unsigned long logType, char* prompt, unsigned char* data, unsigned long len);


#define ERROR(...) \
	do{\
		LogString(LOG_ERROR,__VA_ARGS__);\
	}while(0);

#define INFO(...) \
	do{\
		LogString(LOG_INFO,__VA_ARGS__);\
	}while(0);
	
#define WARN(...) \
	do{\
		LogString(LOG_WARNING,__VA_ARGS__);\
	}while(0);	

#endif //__LOG_H_H__



