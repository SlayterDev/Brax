.globl goLed
goLed:
	ldr r0, =0x20200000
	mov r1, #1
	lsl r1, #18
	str r1, [r0,#4]

	mov r1, #1
	lsl r1, #16
	str r1, [r0,#40]
	mov pc, lr

.globl noLed
noLed:
	ldr r0, =0x20200000
	mov r1, #1
	lsl r1, #18
	str r1, [r0,#4]

	mov r1, #1
	lsl r1, #16
	str r1, [r0,#28]
	mov pc, lr

.globl delay
delay:
	mov r2,#0x3F0000
	wait1$:
	sub r2,#1
	cmp r2,#0
	bne wait1$
	mov pc, lr
