#ifndef UTILS_H
#define UTILS_H

typedef int bool;

#define nil ((void*)0)
#define nelem(x) (sizeof(x)/sizeof((x)[0]))

int print(const char *fmt, ...);
void warn(const char *fmt, ...);
void fatal(char *msg, ...);

// prints formatted message to stderr
// prefixed with local time
void warn_log(const char *fmt, ...);
void warn_console(const char *fmt, ...);

char *xstrdup(const char *p);
void *xmemdup(const void *p, int n);

void *xmalloc(int n);
void *xrealloc(void *, int n);
void	xfree(void*);

#endif
