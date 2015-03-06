#ifndef SCHEDULER_C
#define SCHEDULER_C

#include "lib/common.h"

typedef struct {
	task_t *next;
	int pid;
	unsigned int *stack;
} task_t;

#endif