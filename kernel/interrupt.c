// interrupt.c -- Brad Slayter

#include "interrupt.h"
#include "scheduler.h"
#include "lib/stdio.h"

extern void goLed();
extern void noLed();
extern void toKernel();

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
	kprintf(K_ERROR, "Bad instructions");
	while (1);
}

__attribute__ ((interrupt ("SWI"))) void interruptSWI(void) {
	uint8_t callNo = 0;
	asm volatile("ldrb %0, [lr, #-2]" : "=r"  (callNo));

	if (callNo == 0) {
		kprintf(K_INFO, "Should call here\n");
		toKernel();
	}

	kprintf(K_INFO, "Returned\n");

	asm volatile("mov pc, lr");
}

__attribute__ ((interrupt ("IRQ"))) void interruptIRQ(void) {
	kprintf(K_INFO, "interrupted!!!");

	if (ledStatus) {
		noLed();
		ledStatus = 0;
	} else {
		goLed();
		ledStatus = 1;
	}

	reset_timer();

	// Push all regs into IRQ mode stack
	/*asm volatile("push {R0-R12}");
	// Put LR register from IRQ Mode (PC of interrupted task) in R0
	asm volatile("mov R0, LR");
	// Change to system mode
	asm volatile("cps #0x1F");
	// Push R0 (interrupted pc) onto system stack
	asm volatile("push {R0}");
	// Return to IRQ mode
	asm volatile("cps #0x12");
	// Pop all registers again
	asm volatile("pop {R0-R12}");
	// Return to system mode
	asm volatile("cps #0x1F");
	// Push all regs onto system stack
	asm volatile("push {R0-R12}");
	// Push interrupted LR to system stack
	asm volatile("push {LR}");
	// Copy processor state to R0
	asm volatile("mrs R0, SPSR");
	// Push the processor status to the system stack
	asm volatile("push {R0}");
	// Return to IRQ mode
	asm volatile("cps #0x12");
	// Copy LR to R0
	asm volatile("mov R0, LR");
	// Back to system mode again
	asm volatile("cps #0x1F");

	unsigned long pc;
	unsigned long stack_pointer;

	asm volatile("mov %0, R0\n\t" : "=r" (pc));
	asm volatile("mov %0, sp\n\t" : "=r" (stack_pointer));

	// jump to scheduler
	schedule(stack_pointer, pc);*/
}

__attribute__ ((interrupt ("ABORT"))) void interruptDataAbort(void) {

}

__attribute__ ((interrupt ("ABORT"))) void interruptPrefetchAbort(void) {

}

void reset_timer() {
	*armTimerIRQClear = 0;
}

void init_interrupts() {
	asm volatile("mcr p15, 0, %[addr], c12, c0, 0" : : [addr] "r" (&interruptVectors));
	asm volatile("cpsie i");

	*irqEnableBasic = 0x00000001;
	*armTimerLoad = 0x00000400;
	*armTimerControl = 0x000000AA;
}