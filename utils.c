
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "utils.h"

// output is a generic routine to print a format and accompanying values to a specified file object.
// // output returns the number of bytes printed, which is sometimes useful.
static int output(FILE *out, const char *fmt, va_list arg) {
  int n = 0;
  
  fflush(stdout);
  n += vfprintf(out, fmt, arg);
  return n;
}

// print message out to stderr.
// a perlism, I think.
void warn(const char *fmt, ...) {
  va_list arg;
  va_start(arg, fmt);
  output(stderr, fmt, arg);
  va_end(arg);
}

// print message to stdout
int print(const char *fmt, ...) {
  va_list arg;
  int n;
  va_start(arg, fmt);
  n = output(stdout, fmt, arg);
  va_end(arg);
  return n;
}


// create a formatted date from fmt
// and stores results in buf, which is of size n.
int fmt_date(char *buf, int n, char *fmt) {
    struct tm tm;
    time_t now;
    int ret;

    now = time(NULL);
    localtime_r(&now, &tm);

    ret = strftime(buf, n, fmt, &tm);
    return ret;
}

// returns current time in microseconds.
inline unsigned long usec_time(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000 + tv.tv_usec);
}

// returns current time in seconds
inline unsigned long sec_time(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec);
}

// prints message to stderr, prefixed with date and time.
// the time is represented both in the standard format
// and also in microseconds.
void warn_log(const char *fmt, ...) {
    va_list arg;
    int n;
    char buf[30];

    n = fmt_date(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S");
    if (n != 0)
        warn("%s ", buf);

    warn("[%ld] ", sec_time());

    va_start(arg, fmt);
    output(stderr, fmt, arg);
    va_end(arg);
}

// prints message to stderr, prefixed with date/time.
void warn_console(const char *fmt, ...) {
    struct tm tm;
    struct timeval tv;
    char buf[30];
    int n;
    va_list arg;

    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &tm);
   
    n = strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    if (n != 0)
        fprintf(stderr, "%s ", buf);

    va_start(arg, fmt);
    vfprintf(stderr, fmt, arg);
    va_end(arg);
}

// fatal prints an error message to stderr and aborts.
void
fatal(char *msg, ...)
{
  va_list arg;

  va_start(arg, msg);
  output(stderr, msg, arg);
  va_end(arg);
  exit(3);
}

// xmalloc returns a newly allocated zeroed block of n bytes of memory.
// It calls fatal if it runs out of memory.
void*
xmalloc(int n)
{
	void *p;
	
	p = malloc(n);
	if(p == nil)
	  fatal("out of memory");
	memset(p, 0, n);
	return p;
}

// xstrdup returns a newly allocated copy of p.
// It calls fatal if it runs out of memory.
char*
xstrdup(const char *p)
{
  char *x;

  x = strdup(p);
  if(x == nil)
    fatal("out of memory");
  return x;
}

// xmemdup allocates memory of size n to q
// copies an n size chunk of memory from p into q
// and returns q.
void *
xmemdup(const void *p, int n)
{
    void *q;
    q = xmalloc(n);
    memcpy(q, p, n);
    return q;
}

// xrealloc grows the allocation p to n bytes and
// returns the new (possibly moved) pointer.
// It calls fatal if it runs out of memory.
void*
xrealloc(void *p, int n)
{
	p = realloc(p, n);
	if(p == nil)
		fatal("out of memory");
	return p;
}

// xfree frees the result returned by xmalloc, xstrdup, or xrealloc.
void
xfree(void *p)
{
  free(p);
}
