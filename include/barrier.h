// barrier.h -- Brad Slayter

#ifndef BARRIER_H
#define BARRIER_H

#define dmb() asm volatile\
				("mcr p15, #0, %[zero], c7, c10, #5" : : [zero] "r" (0))

#define flushCache() asm volatile\
				("mcr p15, #0, %[zero], c7, c14, #0" : : [zero] "r" (0))

#endif