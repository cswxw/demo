#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

static pthread_mutex_t g_Mutex = PTHREAD_MUTEX_INITIALIZER;

void Mutex_Lock()
{
	pthread_mutex_lock(&g_Mutex);
}

void Mutex_UnLock()
{
	pthread_mutex_unlock(&g_Mutex);
}
