#include "scheduler.h"
#include "interrupt.h"
#include "mm/blk.h"

#include "lib/stdio.h"

extern unsigned int *kickIt(unsigned int *, unsigned int *);
extern unsigned int *spawn(unsigned int *, unsigned int *);
extern void testSpawn(unsigned int *);

static int nextPid = 0;
static task_t *currentTask;
static task_t *taskHead;

#define STACK_SIZE 256

void idle_task() {
	for (;;);
}

void init_tasking() {
	task_t *mainTask = (task_t *)malloc(sizeof(task_t));

	mainTask->pid = nextPid++;
	mainTask->pc = (unsigned long)&idle_task;
	mainTask->times_loaded = 1;

	unsigned long stack_pointer;
	asm volatile("mov %0, SP\n\t" : "=r" (stack_pointer));
	mainTask->stack = stack_pointer;

	currentTask = mainTask;
	taskHead = currentTask;
}

void fork(unsigned long *pc) {
	task_t *task = (task_t *)malloc(sizeof(task_t));

	kprintf(K_INFO, "Forked stack is: %x\n", &task->stack);

	task->pid = nextPid++;
	task->pc = (unsigned long)pc;
	task->times_loaded = 0;

	task_t *temp = currentTask;
	while (temp->next != NULL)
		temp = temp->next;

	temp->next = task;
}

void schedule(unsigned long stack_pointer, unsigned long pc) {
	currentTask->stack = stack_pointer;
	currentTask->pc = pc;

	kprintf(K_INFO, "Process %d going out...\n");

	currentTask = currentTask->next;
	if (currentTask == NULL)
		currentTask = taskHead;

	currentTask->times_loaded++;

	kprintf(K_INFO, "Restoring stack %x for process %d\n", currentTask->stack, currentTask->pid);

	asm volatile("mov sp, %[addr]" : : [addr] "r" ((unsigned long)(currentTask->stack)));

	if (currentTask->times_loaded > 1) {
		reset_timer();
		// pop regs from stack
		asm volatile("pop {R0}");
		asm volatile("msr SPSR_cxsf, R0");
		asm volatile("pop {LR}");
		asm volatile("pop {R0-R12}");

		// Turn on interrupts again
		asm volatile("cpsie i");

		// Pop last reg into pc to resume process
		asm volatile("pop {PC}");
	} else {
		// push the first pc address onto stack
		unsigned long addr = (unsigned long)(currentTask->pc);
		asm volatile("mov R0, %[addr]" : : [addr] "r" (addr));
		asm volatile("push {R0}");

		reset_timer();

		// turn on interrupts
		asm volatile("cpsie i");

		// pop the last register into pc to start the task
		asm volatile("pop {PC}");


	}
}