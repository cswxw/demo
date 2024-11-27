#include "Log.h"

static void event_log(int severity, const char *msg);
static void event_exit(int errcode);
static event_fatal_cb fatal_fn = NULL;
static event_log_cb log_fn = NULL;


void event_set_log_callback(event_log_cb cb)
{
	log_fn = cb;
}
void event_set_fatal_callback(event_fatal_cb cb)
{
	fatal_fn = cb;
}

int evutil_vsnprintf(char *buf, size_t buflen, const char *format, va_list ap)
{
	int r;
	if (!buflen)
		return 0;
#if defined(_MSC_VER) || defined(_WIN32)
	r = _vsnprintf(buf, buflen, format, ap);
	if (r < 0)
		r = _vscprintf(format, ap);
#elif defined(sgi)
	/* Make sure we always use the correct vsnprintf on IRIX */
	extern int      _xpg5_vsnprintf(char * __restrict,
		__SGI_LIBC_NAMESPACE_QUALIFIER size_t,
		const char * __restrict, /* va_list */ char *);

	r = _xpg5_vsnprintf(buf, buflen, format, ap);
#else
	r = vsnprintf(buf, buflen, format, ap);
#endif
	buf[buflen - 1] = '\0';
	return r;
}

int
evutil_snprintf(char *buf, size_t buflen, const char *format, ...)
{
	int r;
	va_list ap;
	va_start(ap, format);
	r = evutil_vsnprintf(buf, buflen, format, ap);
	va_end(ap);
	return r;
}




void
event_logv_(int severity, const char *errstr, const char *fmt, va_list ap)
{
	char buf[1024];
	size_t len;

	if (severity == EVENT_LOG_DEBUG)//&& !event_debug_get_logging_mask_()
		return;

	if (fmt != NULL)
		evutil_vsnprintf(buf, sizeof(buf), fmt, ap);
	else
		buf[0] = '\0';

	if (errstr) {
		len = strlen(buf);
		if (len < sizeof(buf) - 3) {
			evutil_snprintf(buf + len, sizeof(buf) - len, ": %s", errstr);
		}
	}

	event_log(severity, buf);
}

void
event_warn(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	event_logv_(EVENT_LOG_WARN, strerror(errno), fmt, ap);
	va_end(ap);
}
void
event_err(int eval, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	event_logv_(EVENT_LOG_ERR, strerror(errno), fmt, ap);
	va_end(ap);
	event_exit(eval);
}

static void
event_exit(int errcode)
{
	if (fatal_fn) {
		fatal_fn(errcode);
		exit(errcode); /* should never be reached */
	}
	else if (errcode == EVENT_ERR_ABORT_)
		abort();
	else
		exit(errcode);
}


void
event_msgx(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	event_logv_(EVENT_LOG_MSG, NULL, fmt, ap);
	va_end(ap);
}

void
event_debugx_(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	event_logv_(EVENT_LOG_DEBUG, NULL, fmt, ap);
	va_end(ap);
}



static void event_log(int severity, const char *msg)
{
	if (log_fn)
		log_fn(severity, msg);
	else {
		const char *severity_str;
		switch (severity) {
		case EVENT_LOG_DEBUG:
			severity_str = "debug";
			break;
		case EVENT_LOG_MSG:
			severity_str = "msg";
			break;
		case EVENT_LOG_WARN:
			severity_str = "warn";
			break;
		case EVENT_LOG_ERR:
			severity_str = "err";
			break;
		default:
			severity_str = "???";
			break;
		}
		(void)fprintf(stderr, "[%s] %s\n", severity_str, msg);
	}
}