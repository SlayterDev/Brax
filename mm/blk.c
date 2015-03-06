// blk.c -- Brad Slayter

#include "mm/blk.h"

#include "lib/stdio.h"

#define NALLOC 	  4096 // Minimum # of bytes to request from the phys manager
#define PAGE_SIZE 4096 // In other words, a page

#define MEM_DEBUG 0

#define MAX_BRK 0xf0000

static Header base; 		 // Used to get started on first call
static Header *freep = NULL; // Pointer to free list

extern char __end;

void free(void *ap) {
	Header *bp, *p;

	bp = (Header *)ap - 1; // Point to actual header

	if (MEM_DEBUG)
		kprintf(K_DEBUG, "Freeing: %x\n", &bp->blk.next);

	for (p = freep; !(bp > p && bp < p->blk.next); p = p->blk.next) {
		if (p >= p->blk.next && (bp > p || bp < p->blk.next))
			break; // Freed block at start or end of arena
	}

	if (bp + bp->blk.size == p->blk.next) { // Join to upper
		bp->blk.size += p->blk.next->blk.size;
		bp->blk.next = p->blk.next->blk.next;
	} else {
		bp->blk.next = p->blk.next;
	}

	if (p + p->blk.size == bp) { // Join to lower
		p->blk.size += bp->blk.size;
		p->blk.next = bp->blk.next;
	} else {
		p->blk.next = bp;
	}

	freep = p;
}

uint32_t curBrk = 0;

void *sbrk(uint32_t blocks) {
	uint32_t increment = blocks * PAGE_SIZE;

	if (curBrk == 0) {
		kprintf(K_INFO, "HEAP initializing at %x\n", (uint32_t)&__end);

		curBrk = (uint32_t)&__end;
		if (curBrk & 0xFFF) {
			curBrk &= 0xFFFFF000;
			curBrk += 0x1000;
		}
	}

	uint32_t oldBrk = curBrk;

	curBrk += increment;
	if (curBrk >= MAX_BRK) {
		curBrk = oldBrk;
		return (void*)-1;
	}

	return (void *)oldBrk;
}

static Header *morecore(unsigned nu) {
	char *cp;
	Header *up;

	if (nu < NALLOC)
		nu = NALLOC;

	size_t numBlocks = (nu * sizeof(Header))/PAGE_SIZE;
	numBlocks += (numBlocks) ? 0 : 1;

	cp = (char *)sbrk(numBlocks); // Grab moar memory
	if (cp == (char *)0) // No memory left
		return NULL;

	up = (Header *)cp;
	up->blk.size = nu;
	free((void *)(up+1));
	return freep;
}

void *malloc(unsigned nbytes) {
	Header *p, *prevp;
	unsigned nunits;

	nunits = (nbytes + sizeof(Header)-1) / sizeof(Header) + 1;
	if ((prevp = freep) == NULL) { // Free List doesn't exist yet
		base.blk.next = freep = prevp = &base;
		base.blk.size = 0;
	}

	for (p = prevp->blk.next; ; prevp = p, p = p->blk.next) {
		if (p->blk.size >= nunits) { 	 // Big enough space
			if (p->blk.size == nunits) { // Exactly big enough
				prevp->blk.next = p->blk.next;
			} else { // return tail end of free block
				p->blk.size -= nunits;
				p += p->blk.size;
				p->blk.size = nunits;
			}
			freep = prevp;

			if (MEM_DEBUG)
				kprintf(K_DEBUG, "Allocating: %x\n", &p->blk.next);
			
			return (void *)(p+1);
		}
		if (p == freep) { // Wrapped around the list
			if ((p = morecore(nunits)) == NULL)
				return NULL; // Out of memory
		}
	}

	return NULL; // Something bad happened
}

void *calloc(size_t n, unsigned size) {
	return malloc(size*n);
}