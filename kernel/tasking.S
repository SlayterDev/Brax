.global toKernel
toKernel:
	/* Save user state */
	mov r0, sp
	stmdb r0!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}

	/* Load kernel state */
	pop {r4, r5, r6, r7, r8, r9, r10, r11, ip, lr}
	msr cpsr, ip

	bx lr

.global activate
activate:
	ldmfd r0!, {ip, lr}
	msr spsr, ip

	msr cpsr_c, #0xDF /* System Mode */
	mov sp, r0
	pop {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, fp, ip, lr}
	msr cpsr_c, #0xD3 /* Supervisor Mode */

	movs pc, lr