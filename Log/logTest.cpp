//download from csdn
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _MSC_VER
    #pragma warning(disable:4996)
    #include <windows.h>
    #include <io.h>
#else
    #include <unistd.h>
    #include <sys/time.h>
    #include <pthread.h>
    #define  CRITICAL_SECTION   pthread_mutex_t
    #define  _vsnprintf         vsnprintf
#endif
//Log{
#define MAXLOGSIZE 20000000
#define MAXLINSIZE 16000
#include <time.h>
#include <sys/timeb.h>
#include <stdarg.h>
char logfilename1[]="MyLog1.log";
char logfilename2[]="MyLog2.log";
static char logstr[MAXLINSIZE+1];
char datestr[16];
char timestr[16];
char mss[4];
CRITICAL_SECTION cs_log;
FILE *flog;
#ifdef _MSC_VER
void Lock(CRITICAL_SECTION *l) {
    EnterCriticalSection(l);
}
void Unlock(CRITICAL_SECTION *l) {
    LeaveCriticalSection(l);
}
#else
void Lock(CRITICAL_SECTION *l) {
    pthread_mutex_lock(l);
}
void Unlock(CRITICAL_SECTION *l) {
    pthread_mutex_unlock(l);
}
#endif
void LogV(const char *pszFmt,va_list argp) {
    struct tm *now;
    struct timeb tb;
 
    if (NULL==pszFmt||0==pszFmt[0]) return;
    _vsnprintf(logstr,MAXLINSIZE,pszFmt,argp);
    ftime(&tb);
    now=localtime(&tb.time);
    sprintf(datestr,"%04d-%02d-%02d",now->tm_year+1900,now->tm_mon+1,now->tm_mday);
    sprintf(timestr,"%02d:%02d:%02d",now->tm_hour     ,now->tm_min  ,now->tm_sec );
    sprintf(mss,"%03d",tb.millitm);
    printf("%s %s.%s %s",datestr,timestr,mss,logstr);
    flog=fopen(logfilename1,"a");
    if (NULL!=flog) {
        fprintf(flog,"%s %s.%s %s",datestr,timestr,mss,logstr);
        if (ftell(flog)>MAXLOGSIZE) {
            fclose(flog);
            if (rename(logfilename1,logfilename2)) {
                remove(logfilename2);
                rename(logfilename1,logfilename2);
            }
        } else {
            fclose(flog);
        }
    }
}
void Log(const char *pszFmt,...) {
    va_list argp;
 
    Lock(&cs_log);
    va_start(argp,pszFmt);
    LogV(pszFmt,argp);
    va_end(argp);
    Unlock(&cs_log);
}
//Log}
int main(int argc,char * argv[]) {
    int i;
#ifdef _MSC_VER
    InitializeCriticalSection(&cs_log);
#else
    pthread_mutex_init(&cs_log,NULL);
#endif
    for (i=0;i<10000;i++) {
        Log("This is a Log %04d from FILE:%s LINE:%d\n",i, __FILE__, __LINE__);
    }
#ifdef _MSC_VER
    DeleteCriticalSection(&cs_log);
#else
    pthread_mutex_destroy(&cs_log);
#endif
    return 0;
}



