#ifndef SCHEDULER_C
#define SCHEDULER_C

#include "lib/common.h"

typedef struct task {
	struct task *next;
	int pid;
	unsigned long stack;
	unsigned long pc;
	unsigned int times_loaded;
} task_t;

void init_tasking();
void fork(unsigned long *pc);
void spawnTask(void (*start)(void));
void schedule(unsigned long stack_pointer, unsigned long pc);

#endif