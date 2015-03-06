// blk.h -- Brad Slayter

/*

	This uses a malloc and free implementation from "The C Programming
	language" by Brian W. Kernighan and Dennis M. Ritchie

*/

#ifndef BLK_H
#define BLK_H

#include "lib/common.h"

typedef long Align; // To align our blocks

union header {
	struct {
		union header *next; // Pointer to next memory block
		unsigned size;		// Size of the block
	} blk;
	Align x; // Force alignment
};

typedef union header Header;

void *malloc(unsigned nbytes);
void *calloc(size_t n, unsigned size);
void free(void *ap);

#endif