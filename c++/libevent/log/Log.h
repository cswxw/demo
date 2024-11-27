#ifndef __LOG__H__
#define __LOG__H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#define EVENT_LOG_DEBUG 0
#define EVENT_LOG_MSG   1
#define EVENT_LOG_WARN  2
#define EVENT_LOG_ERR   3


typedef void(*event_log_cb)(int severity, const char *msg);
typedef void(*event_fatal_cb)(int err);

void event_warn(const char *fmt, ...);
void event_err(int eval, const char *fmt, ...);

#define EVENT_ERR_ABORT_ ((int)0xdeaddead)
#endif //__LOG__H__