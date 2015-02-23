.section ".text.boot"

// Make start global
.globl _start

// Entry point for kernel
// r15 -> should begin at 0x8000
// r0  -> 0x00000000
// r1  -> 0x00000C42
// r2  -> 0x00000100 - start of ATAGS
// These registers should be preserved as args for kmain
_start:
	// Setup Stack
	mov sp, #0x8000

	// Clear out bss
	ldr r4, =__bss_start
	ldr r9, =__bss_end
	mov r5, #0
	mov r6, #0
	mov r7, #0
	mov r8, #0
	b 		2f

1:
	// Store multiple at r4
	stmia r4!, {r5-r8}

	// loop to the end of __bss_end
2:
	cmp r4, r9
	blo 1b

	// Call kernel main
	ldr r3, =kernel_main
	blx r3

	// halt
halt:
	wfe
	b halt
