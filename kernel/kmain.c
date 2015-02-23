#include <stddef.h>
#include <stdint.h>

extern void goLed();
extern void noLed();
extern void delay();

int kernel_main(uint32_t r0, uint32_t r1, uint32_t atags) {
	(void)r0;
	(void)r1;
	(void)atags;

	//goLed();
	uint8_t ledStatus = 0;
	while (1) {
		delay();
	//	uint32_t cnt = 0;
	//	if (cnt % 10000 == 0) {
			if (ledStatus) {
				noLed();
				ledStatus = 0;
			} else {
				goLed();
				ledStatus = 1;
			}
	//	}

	//	cnt++;
	}

	while (1);

	return 0;
}