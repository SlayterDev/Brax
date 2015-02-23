// common.h -- Brad Slayter

#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdint.h>

// Memory utils
void *memmove(void *dest, const void *source, size_t len);
int memcmp(const void *aptr, const void *bptr, size_t len);
void *memset(void *buf, int value, size_t len);
void *memcpy(void *restrict dest, const void *restrict source, size_t len);
void memclr(void *dest, size_t len);

// String utils
size_t strlen(const char *str);

#endif