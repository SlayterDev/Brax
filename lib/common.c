// common.c -- Brad Slayter

#include "lib/common.h"

void *memmove(void *dest, const void *source, size_t len) {
	unsigned char *d = (unsigned char *)dest;
	const unsigned char *s = (const unsigned char *)source;
	if (d < s) {
		for (size_t i = 0; i < len; i++)
			d[i] = s[i];
	} else {
		for (size_t i = len; i != 0; i--)
			d[i-1] = s[i-1];
	}

	return dest;
}

int memcmp(const void *aptr, const void *bptr, size_t len) {
	const unsigned char *a = (const unsigned char *)aptr;
	const unsigned char *b = (const unsigned char *)bptr;
	for (size_t i = 0; i < len; i++) {
		if (a[i] < b[i])
			return -1;
		else if (b[i] < a[i])
			return 1;

	}

	return 0;
}

void *memset(void *buf, int value, size_t len) {
	unsigned char *b = (unsigned char *)buf;
	for (size_t i = 0; i < len; i++) {
		b[i] = value;
	}

	return b;
}

void *memcpy(void *restrict dest, const void *restrict source, size_t len) {
	unsigned char *d = (unsigned char *)dest;
	const unsigned char *s = (const unsigned char *)source;
	for (size_t i = 0; i < len; i++) {
		d[i] = s[i];
	}

	return dest;
}

void memclr(void *dest, size_t len) {
	unsigned char *d = (unsigned char *)dest;
	for (size_t i = 0; i < len; i++) {
		d[i] = 0;
	}
}

size_t strlen(const char *str) {
	size_t len = 0;
	while (str[len])
		len++;

	return len;
}