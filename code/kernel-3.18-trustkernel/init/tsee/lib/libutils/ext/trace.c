/*
 *
 * Copyright (C) 2015 - 2017 TrustKernel Team, Shanghai Pingbo Information Technology Corporation Limited - All Rights Reserved
 *
 *      https://www.trustkernel.com
 *
 * This file is part of T6 and TSEE.
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <trace.h>
#include <util.h>

#if (TRACE_LEVEL > 0)

#if (TRACE_LEVEL < TRACE_MIN) || (TRACE_LEVEL > TRACE_MAX)
#error "Invalid value of TRACE_LEVEL"
#endif

void trace_set_level(int level)
{
	if (((int)level >= TRACE_MIN) && (level <= TRACE_MAX))
		trace_level = level;
	else
		trace_level = TRACE_MAX;
}

int trace_get_level(void)
{
	return trace_level;
}

static const char *trace_level_to_string(int level, bool level_ok)
{
	static const char lvl_strs[][4] = {
		"UKN", "ERR", "INF", "DBG", "FLW" };
	int l = 0;

	if (!level_ok)
		return "MSG";

	if ((level >= TRACE_MIN) && (level <= TRACE_MAX))
		l = level;

	return lvl_strs[l];
}

/* Format trace of user ta. Inline with kernel ta */
void trace_printf(const char *function, int line, int level, bool level_ok,
		  const char *fmt, ...)
{
	va_list ap;
	char buf[MAX_PRINT_SIZE];
	size_t boffs = 0;
	int res;

	if (level_ok && level > trace_level)
		return;

	if (function) {
		int thread_id = trace_ext_get_thread_id();

		if (thread_id >= 0)
			res = snprintf(buf, sizeof(buf), "%s [0x%x] %s:%s:%d: ",
				       trace_level_to_string(level, level_ok),
				       thread_id, trace_ext_prefix,
				       function, line);
		else
			res = snprintf(buf, sizeof(buf), "%s %s:%s:%d: ",
				       trace_level_to_string(level, level_ok),
				       trace_ext_prefix, function, line);
		if (res < 0)
			return; /* "Can't happen" */
		boffs = res;
	}

	va_start(ap, fmt);
	res = vsnprintf(buf + boffs, sizeof(buf) - boffs, fmt, ap);
	va_end(ap);
	if (res > 0)
		boffs += res;

	if (boffs >= sizeof(buf)) {
		boffs = sizeof(buf) - 2;
		/* Make there's a newline at the end */
		buf[boffs] = '\n';
	} else if (buf[boffs - 1] != '\n') {
		/* Append a newline */
		buf[boffs] = '\n';
		buf[boffs + 1] = '\0';
	}

	trace_ext_puts(buf);
}

#else

/*
 * In case we have a zero or negative trace level when compiling optee_os, we
 * have to add stubs to trace functions in case they are used with TA having a
 * non-zero trace level
 */

void trace_set_level(int level __unused)
{
}

int trace_get_level(void)
{
	return 0;
}

void trace_printf(const char *function __unused, int line __unused,
		  int level __unused, bool level_ok __unused,
		  const char *fmt __unused, ...)
{
}

#endif

#if (TRACE_LEVEL >= TRACE_DEBUG)
struct strbuf {
	char buf[MAX_PRINT_SIZE];
	char *ptr;
};

static int __printf(2, 3) append(struct strbuf *sbuf, const char *fmt, ...)
{
	int left;
	int len;
	va_list ap;

	if (sbuf->ptr == NULL)
		sbuf->ptr = sbuf->buf;
	left = sizeof(sbuf->buf) - (sbuf->ptr - sbuf->buf);
	va_start(ap, fmt);
	len = vsnprintf(sbuf->ptr, left, fmt, ap);
	va_end(ap);
	if (len < 0) {
		/* Format error */
		return 0;
	}
	if (len >= left) {
		/* Output was truncated */
		return 0;
	}
	sbuf->ptr += MIN(left, len);
	return 1;
}

void dhex_dump(const char *function, int line, int level,
	       const void *buf, int len)
{
	int i;
	int ok;
	struct strbuf sbuf;
	char *in = (char *)buf;

	if (level <= trace_level) {
		sbuf.ptr = NULL;
		for (i = 0; i < len; i++) {
			ok = append(&sbuf, "%02x ", in[i]);
			if (!ok)
				goto err;
			if ((i % 16) == 7) {
				ok = append(&sbuf, " ");
				if (!ok)
					goto err;
			} else if ((i % 16) == 15) {
				trace_printf(function, line, level, true, "%s",
					     sbuf.buf);
				sbuf.ptr = NULL;
			}
		}
		if (sbuf.ptr) {
			/* Buffer is not empty: flush it */
			trace_printf(function, line, level, true, "%s",
				     sbuf.buf);

		}
	}
	return;
err:
	DMSG("Hex dump error");
}
#else

/*
 * In case we have trace level less than debug when compiling optee_os, we have
 * to add stubs to trace functions in case they are used with TA having a
 * a higher trace level
 */

void dhex_dump(const char *function __unused, int line __unused,
	       int level __unused,
	       const void *buf __unused, int len __unused)
{
}

#endif
