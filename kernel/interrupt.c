// interrupt.c -- Brad Slayter

#include "interrupt.h"

extern void goLed();
extern void noLed();

static volatile unsigned int *irqEnableBasic = (unsigned int *)0x2000b218;
static volatile unsigned int *armTimerIRQClear = (unsigned int *)0x2000b40c;
static volatile unsigned int *armTimerLoad = (unsigned int *)0x2000b400;
static volatile unsigned int *armTimerControl = (unsigned int *)0x2000b408;

int ledStatus = 0;

__attribute__ ((naked, aligned(32))) static void interruptVectors(void) {
	asm volatile("b badException\n"
		"b badException\n"
		"b interruptSWI\n"
		"b interruptPrefetchAbort\n"
		"b interruptDataAbort\n"
		"b badException\n"
		"b interruptIRQ\n"
		"b badException\n");
}

__attribute__ ((naked)) void badException(void) {
	while (1);
}

__attribute__ ((interrupt ("SWI"))) void interruptSWI(void) {
	register unsigned int addr;
	register unsigned int swiNo;

	asm volatile("mov %[addr], lr" : [addr] "=r" (addr));

	addr -= 4;
	swiNo = *((unsigned int *) addr) & 0x00FFFFFF;


}

__attribute__ ((interrupt ("IRQ"))) void interruptIRQ(void) {
	*armTimerIRQClear = 0;
	if (ledStatus) {
		noLed();
		ledStatus = 0;
	} else {
		goLed();
		ledStatus = 1;
	}
}

__attribute__ ((interrupt ("ABORT"))) void interruptDataAbort(void) {

}

__attribute__ ((interrupt ("ABORT"))) void interruptPrefetchAbort(void) {

}

void init_interrupts() {
	asm volatile("mcr p15, 0, %[addr], c12, c0, 0" : : [addr] "r" (&interruptVectors));
	asm volatile("cpsie i");

	*irqEnableBasic = 0x00000001;
	*armTimerLoad = 0x00000400;
	*armTimerControl = 0x000000AA;
}