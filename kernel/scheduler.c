#include "scheduler.h"
#include "mm/blk.h"

#include "lib/stdio.h"

extern unsigned int *kickIt(unsigned int *, unsigned int *);
extern unsigned int *spawn(unsigned int *, unsigned int *);
extern void testSpawn(unsigned int *);

static int nextPid = 0;
volatile task_t *currentTask;
volatile task_t *taskHead;

#define STACK_SIZE 256

void switchTask();

void taskC() {
	kprintf(K_INFO, "I am the great and powerful task C. pid: %d\n", currentTask->pid);
	switchTask();
	while(1);
}

void taskB() {
	kprintf(K_INFO, "By the dark voodoo magic, we are now idling\n");
	switchTask();
	while(1);
}

void taskA() {
	kprintf(K_INFO, "We now exist in pid %d\n", currentTask->pid);
	//spawnTask(&taskC);
	switchTask();
	kprintf(K_INFO, "We came back? pid: %d\n", currentTask->pid);
	while(1);
}

void switchTask() {
	task_t *lastTask = (task_t*)currentTask;
	currentTask = currentTask->next;
	if (!currentTask)
		currentTask = taskHead;

	kprintf(K_INFO, "Switch attempt for pid: %d\n", currentTask->pid);
	if (currentTask->pid > 0) {
		kprintf(K_INFO, "Dest: %x\n", currentTask->stack + STACK_SIZE - 8);
		lastTask->stack = kickIt(lastTask->stack, currentTask->stack);
	} else {
		unsigned int *stackStart = currentTask->stack + STACK_SIZE - 8;
		kprintf(K_INFO, "Switch to dest: %x\n", stackStart[0]);
		lastTask->stack = spawn(lastTask->stack, stackStart);
	}
}

void spawnTask(void (*start)(void)) {
	unsigned int *stack = (unsigned int *)malloc(sizeof(unsigned int) * STACK_SIZE);
	task_t *newTask = (task_t *)malloc(sizeof(task_t));

	for (int i = 0; i < STACK_SIZE; i++) {
		stack[i] = 0;
	}

	newTask->pid = nextPid++;
	newTask->stack = stack;

	task_t *temp = (task_t*)currentTask;
	while (temp->next)
		temp = temp->next;

	temp->next = newTask;
	newTask->next = 0;

	unsigned int *stackStart = stack + STACK_SIZE - 8;
	stackStart[0] = (unsigned int)start;

	// switch to the new task
	temp = (task_t*)currentTask;
	currentTask = newTask;

	kprintf(K_INFO, "Kick it! Dest: %x\n", stackStart[0]);
	spawn(temp->stack, stackStart);
}

void initTasking() {
	kprintf(K_INFO, "Allocate task\n");
	task_t *idleTask = (task_t *)malloc(sizeof(task_t));

	currentTask = taskHead = idleTask;
	currentTask->pid = nextPid++;
	currentTask->next = 0;
	currentTask->stack = (unsigned int *)malloc(sizeof(unsigned int) * STACK_SIZE);
	
	unsigned int *idleEntry = currentTask->stack + STACK_SIZE - 8;
	idleEntry[0] = (unsigned int)&taskB;
	kprintf(K_INFO, "Idle address: %x\n", idleEntry[0]);

	//testSpawn(idleEntry);

	spawnTask(&taskA);

	kprintf(K_INFO, "By voodoo magic we made it back here\n");
	while(1);
}