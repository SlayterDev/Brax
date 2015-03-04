#include <stddef.h>
#include <stdint.h>

#include "interrupt.h"
#include "framebuffer.h"

#include "lib/stdio.h"

extern void goLed();
extern void noLed();
extern void delay();

int kernel_main(uint32_t r0, uint32_t r1, uint32_t atags) {
	(void)r0;
	(void)r1;
	(void)atags;

	fb_init();
	drawStuff();
	init_interrupts();

	kprintf(K_OK, "Kernel boots ok ;D \\ H^H _ ` | ~ { } #\n");
	kprintf(K_INFO, "The meaning of life is %d\n", 42);

	asm volatile("svc 0");

	kprintf(K_OK, "Annnnnd we're back.\n");

	while (1);

	return 0;
}