#include "Log.h"
#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h> 


//#define _DEBUG
#define MAX_LOG_SIZE 32*1024*1026


void LogString(unsigned long logType, char* fmt, ...)
{
#ifndef _DEBUG
	return;
#endif //_DEBUG
	
	char LogInfo[4096];
	char LogFile[256];
	char Level[16];
	time_t now;
	struct tm* timenow;
	struct stat st;
	
	FILE* f = NULL;
	
	char szTime[128] = {0};
	int nLen = 0;
	
	va_list arg;
	
	strcpy(LogFile, LOG_DIR);
	switch(logType) {
	case LOG_INFO:
		strcpy(LogFile+strlen(LogFile), "app_info.log");
		strcpy(Level,"[info   ] ");
		break;
	case LOG_WARNING:
		strcpy(LogFile+strlen(LogFile), "app_warning.log");
		strcpy(Level,"[warning] ");
		break;
	case LOG_ERROR:
		strcpy(LogFile+strlen(LogFile), "app_error.log");
		strcpy(Level,"[error  ] ");
		break;
	default:
		break;
	}



	va_start(arg, fmt);
	vsprintf(LogInfo, fmt, arg);
	va_end(arg);



	time(&now);
	timenow = localtime(&now);
	nLen = sprintf(szTime, "[%04d-%02d-%02d %02d:%02d:%02d] ",
				   timenow->tm_year + 1900, timenow->tm_mon + 1, timenow->tm_mday,
				   timenow->tm_hour, timenow->tm_min, timenow->tm_sec);
	
	//真真
	if (stat(LogFile, &st)==0 && st.st_size>MAX_LOG_SIZE)
	{
		unlink(LogFile);
	}

	f=fopen(LogFile, "a+");
	if (f)
	{
		fwrite(szTime, 1, nLen, f);
		fwrite(Level, 1, strlen(Level), f);
		fwrite(LogInfo, 1, strlen(LogInfo), f);
		fwrite("\n", 1, 1, f);
		fclose(f);
	}
	fprintf(stderr, "%s%s%s", szTime,Level,LogInfo);
    fprintf(stderr, "\n");
}

void LogData(unsigned long logType, char* prompt, unsigned char* data, unsigned long len)
{
#ifndef _DEBUG
	return;
#endif //_DEBUG
	
	//TODO:
	FILE *pFile;
	unsigned long i;
	char LogFile[256];
	char szBuf[256] = {0};
	char Level[16];
	time_t now;
	struct tm* timenow;
	int nLen = 0;
	struct stat st;
	
	//真真真
	strcpy(LogFile, LOG_DIR);
	switch(logType) {
	case LOG_INFO:
		strcpy(LogFile+strlen(LogFile), "app_info.log");
		strcpy(Level,"[info   ] ");
		break;
	case LOG_WARNING:
		strcpy(LogFile+strlen(LogFile), "app_warning.log");
		strcpy(Level,"[warning] ");
		break;
	case LOG_ERROR:
		strcpy(LogFile+strlen(LogFile), "app_error.log");
		strcpy(Level,"[error  ] ");
		break;
	default:
		break;
	}
	
	time(&now);
	timenow = localtime(&now);
	nLen = sprintf(szBuf, "[%04d-%02d-%02d %02d:%02d:%02d] %s %s\n",
		timenow->tm_year + 1900, timenow->tm_mon + 1, timenow->tm_mday,
		timenow->tm_hour, timenow->tm_min, timenow->tm_sec,Level, prompt);
	
	//真真
	if (stat(LogFile, &st)==0 && st.st_size>MAX_LOG_SIZE)
	{
		unlink(LogFile);
	}

	if((pFile=fopen(LogFile,"a+"))!=NULL)
	{
		fprintf(pFile, szBuf);
		for(i=0;i<len;i++)
		{
			if(i!=0 && i%32 == 0)
				fprintf(pFile,"\n");
			
			if(*(data+i)<=0xf){
				fprintf(pFile,"0%x ",*(data+i));
			}
			else{
				fprintf(pFile,"%x ",*(data+i));
			}
		}
		fprintf(pFile,"\n");
		fclose(pFile);
	}
}


