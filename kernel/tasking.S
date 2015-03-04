.global toKernel
toKernel:
	/* Save user state */
	mov r0, sp
	stmdb r0!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}

	/* Load kernel state */
	pop {r4, r5, r6, r7, r8, r9, r10, r11, ip, lr}
	msr cpsr_all, ip

	bx lr

.global activate
activate:
	/* Save kernel state */
	mrs ip, cpsr
	push {r4, r5, r6, r7, r8, r9, r10, r11, ip, lr}

	/* Switch to process stack */
	mov sp, r0
	mov r0, #3
	msr control, r0
	isb

	/* load user state */
	pop {r4, r5, r6, r7, r8, r9, r10, r11, lr}

	/* jump to user task */
	bx lr