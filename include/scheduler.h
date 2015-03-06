#ifndef SCHEDULER_C
#define SCHEDULER_C

#include "lib/common.h"

typedef struct task {
	struct task *next;
	int pid;
	unsigned int *stack;
} task_t;

void initTasking();
void spawnTask(void (*start)(void));

#endif