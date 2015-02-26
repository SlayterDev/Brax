// stdio.h -- Brad Slayter

#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>
#include "lib/common.h"

enum info_levels {
	K_ERROR = 0,
	K_OK 	= 1,
	K_WARN  = 2,
	K_INFO  = 3,
	K_NONE	= 4,
	K_DEBUG = 5
};

void kprintf(int level, const char *format, ...);

char getch();

#endif