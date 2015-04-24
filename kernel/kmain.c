#include <stddef.h>
#include <stdint.h>

#include "interrupt.h"
#include "framebuffer.h"
#include "scheduler.h"

#include "lib/stdio.h"

#include "mm/blk.h"

extern void goLed();
extern void noLed();
extern void delay();
extern unsigned int *activate(unsigned int *stack);
extern void yeild();

#define STACK_SIZE 256
#define THREAD_PSP	0xFFFFFFFD

void sample_process_1() {
	kprintf(K_NONE, "Sample process 1 has entered the game!\n");

	int n = 10, first = 0, second = 1, next, c;

	kprintf(K_NONE, "Computing the first 1000 fibonachi numbers...\n");

	for (c = 0; c < n; c++) {
		if (c <= 1) {
			next = c;
		} else {
			next = first + second;
			first = second;
			second = next;
		}
		kprintf(K_NONE, "FIB: %d\n", next);
	}
}

void sample_process_2() {
	kprintf(K_NONE, "Sample process 2 has entered the game!\n");

	while (1) {

		// Bad sleep
		int n = 300000, i = 0;
		while (i++ < n);

		kprintf(K_NONE, "Hello, world");
	}
}

int kernel_main(uint32_t r0, uint32_t r1, uint32_t atags) {
	(void)r0;
	(void)r1;
	(void)atags;

	fb_init();
	drawStuff();

	kprintf(K_OK, "Kernel boots ok ;D \\ H^H _ ` | ~ { } #\n");
	kprintf(K_INFO, "The meaning of life is %d\n", 42);

	//init_tasking();

	//fork((unsigned long *)&sample_process_1);
	//fork((unsigned long *)&sample_process_2);
	//kprintf(K_INFO, "Processes forked\n");

	init_interrupts();

	sample_process_1();

	asm volatile("swi #0x1");

	while (1);

	return 0;
}