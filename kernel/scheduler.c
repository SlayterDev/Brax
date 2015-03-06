#include "scheduler.h"

static int nextPid = 0;
task_t *currentTask;
task_t *taskHead;

#define STACK_SIZE 256

void switchTask() {
	task_t *lastTask = currentTask;
	currentTask = currentTask->next;

	lastTask->stack = kickIt(lastTask->stack, currentTask->stack);
}

void spawnTask(void (*start)(void)) {
	unsigned int stack[STACK_SIZE];
	task_t *newTask;

	newTask->pid = nextPid++;
	newTask->stack = stack;

	task_t *temp = currentTask;
	while (temp->next)
		temp = temp->next;

	temp->next = newTask;

	unsigned int *stackStart = stack + STACK_SIZE - 8;
	stackStart[0] = (unsigned int)start;

	// switch to the new task
	temp = currentTask;
	currentTask = newTask;
	kickIt(temp->stack, newTask->stack);
}

void initTasking() {
	task_t *idleTask;

	currentTask = taskHead = idleTask;
	currentTask->pid = nextPid++;
}