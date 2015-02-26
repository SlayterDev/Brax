// string.h -- Brad Slayter

#ifndef STRING_H
#define STRING_H

#include "common.h"

int strcmp(const char *str1, const char *str2);
char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *source);
char toupper(char c);
char tolower(char c);
char *strchr(char *str, int c);

#endif