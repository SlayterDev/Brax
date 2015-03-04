#include <stddef.h>
#include <stdint.h>

#include "interrupt.h"
#include "framebuffer.h"

#include "lib/stdio.h"

extern void goLed();
extern void noLed();
extern void delay();
extern unsigned int *activate(unsigned int *stack);

#define STACK_SIZE 256
#define THREAD_PSP	0xFFFFFFFD

unsigned int *createTask(unsigned int *stack, void (*start)(void)) {
	static int first = 1;

	for (int i = 0; i < STACK_SIZE; i++) { // clear the stack
		stack[i] = 0;
	}
	kprintf(K_INFO, "Stack is: %x\n", stack[0]);

	unsigned int *stack_start = stack + STACK_SIZE - 16;
	if (first) {
		stack_start[0] = 0x10; // User mode
		stack_start[1] = (unsigned int)start;
		first = 0;
		kprintf(K_INFO, "Target: %x, loaded: %x\n", (unsigned int)start, stack_start[1]);
	} else {
		stack[8] = (unsigned int)THREAD_PSP;
		stack[15] = (unsigned int)start;
		stack[16] = (unsigned int)0x01000000;
	}
	stack = activate(stack_start);

	return stack;
}

void task1() {
	kprintf(K_OK, "Task 1 churning away in user land. Take it away kernel\n");
	asm volatile("svc 0");
}

int kernel_main(uint32_t r0, uint32_t r1, uint32_t atags) {
	(void)r0;
	(void)r1;
	(void)atags;

	fb_init();
	drawStuff();
	init_interrupts();

	kprintf(K_OK, "Kernel boots ok ;D \\ H^H _ ` | ~ { } #\n");
	kprintf(K_INFO, "The meaning of life is %d\n", 42);

	unsigned int user_stacks[2][STACK_SIZE];
	unsigned int *usertasks[2];

	usertasks[0] = createTask(user_stacks[0], &task1);
	kprintf(K_OK, "Kernel is back!\n");

	while (1);

	return 0;
}