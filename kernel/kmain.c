#include <stddef.h>
#include <stdint.h>

extern void goLed();

int kernel_main(uint32_t r0, uint32_t r1, uint32_t atags) {
	(void)r0;
	(void)r1;
	(void)atags;

	goLed();

	while (1);

	return 0;
}