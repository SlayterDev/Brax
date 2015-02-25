#include <stddef.h>
#include <stdint.h>

#include "interrupt.h"
#include "framebuffer.h"

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

	while (1);

	return 0;
}