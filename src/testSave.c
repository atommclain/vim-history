#include <stdarg.h>
#include <stdio.h>
#include <unix.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "testSave.h"

#if defined(THINK_C)
#define USE_DEBUGA
#endif

void debugA(const char *format, ...);

void
testSave()
{
	char *path = "PlusHD:Vim:ponchos";
	// think c flags = 22
	// cw2 flags     = 1536
	// cw5 flags     = 1537
	// mpw flags     = 1282
	int flags = O_RDWR|O_CREAT|O_EXCL;
	int fd;
	
	debugA("path is: %s    flags are: %i\n", path, flags);
	debugA("pre-error check:    errno: %d errorMessage: %s\n", errno, strerror(errno));
	
	fd = open(path, flags);
	debugA("fd is %i\n", fd);
	
	if (fd < 0) {
		debugA("errno: %d errorMessage: %s\n", errno, strerror(errno));
		// Think C file exists -48
		// MPW     file exists  17
		// CWP 2&5 System 6, always return -50
	} else {
		// CWP 2&5 don't care if file already exists
		debugA("file %s created!\n", path);
	}
}

#ifdef USE_DEBUGA

#define ERROR_STRING_SIZE	1024
static char err_str[ERROR_STRING_SIZE];

void
debugA(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	vsprintf(err_str, format, ap);
	//printf(err_str);
	va_end(ap);
}
#endif