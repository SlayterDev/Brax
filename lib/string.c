// string.c -- Brad Slayter

#include "lib/string.h"

int strcmp(const char *str1, const char *str2) {
	int i = 0;
	int ret;
	while (str1[i]) {
		if (str1[i] == str2[i]) {
			ret = 0;
		} else if (str1[i] < str2[i]) {
			ret = -1;
			break;
		} else if (str1[i] > str2[i]) {
			ret = 1;
			break;
		}
		i++;
	}

	if ((!str1[i] && str2[i]) && ret == 0) {
		ret = -1;
	}

	return ret;
}

char *strcpy(char *dest, const char *src) {
    int i = 0;
    while ((dest[i] = src[i]) != '\0')
    	i++;

    return dest;
}

char *strcat(char *dest, const char *source) {
	char *a;
	for (a = dest; *a; a++)
		;

	for (; *source; a++, source++)
		*a = *source;
	*a = 0;

	return dest;
}

char toupper(char c) {
	if (c >= 'a' && c <= 'z')
		return 'A' + (c - 'a');
	else
		return c;
}

char tolower(char c) {
	if (c >= 'A' && c <= 'Z')
		return 'a' + (c - 'A');
	else
		return c;
}

char *strchr(char *str, int c) {
	do {
		if (*str == c)
			return (char *)str;
	} while (*str++);

	return 0;
}