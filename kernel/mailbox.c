// mailbox.c -- Brad Slayter

#include "mailbox.h"
#include "barrier.h"

static volatile uint32_t *MAILBOX0READ = (uint32_t *)0x2000B880;
static volatile uint32_t *MAILBOX0STATUS = (uint32_t *)0x2000B898;
static volatile uint32_t *MAILBOX0WRITE = (uint32_t *)0x2000B8A0;

#define MAILBOX_FULL 0x80000000
#define MAILBOX_EMPTY 0x40000000

uint32_t readMailbox(uint32_t channel) {
	uint32_t count = 0;
	uint32_t data;

	// Loop until something is recieved
	while (1) {
		while (*MAILBOX0STATUS & MAILBOX_EMPTY) {
			flushCache();

			// Here is our timeout counter
			if (count++ > (1<<25)) {
				return 0xFFFFFFFF;
			}
		}

		// Read the data
		dmb();
		data = *MAILBOX0READ;
		dmb();

		if ((data & 15) == channel) {
			return data;
		}
	}
}

void writeMailbox(uint32_t channel, uint32_t data) {
	while (*MAILBOX0STATUS & MAILBOX_FULL) {
		flushCache();
	}

	dmb();
	*MAILBOX0WRITE = (data | channel);
}