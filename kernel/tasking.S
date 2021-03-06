.global yeild
yeild:
	svc 0
	bx lr

.global toKernel
toKernel:
	/* Save user state */
	msr cpsr_c, #0xDF /* System mode */
	push {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, fp, ip, lr}
	mov r0, sp
	msr cpsr_c, #0xD3 /* Supervisor mode */

	mrs ip, spsr
	stmfd r0!, {ip, lr}

	/* load kernel state */
	pop {r4, r5, r6, r7, r8, r9, r10, fp, ip, lr}
	mov sp, ip
	bx lr

.global activate
activate:
	/* save kernel state */
	mov ip, sp
	push {r4, r5, r6, r7, r8, r9, r10, fp, ip, lr}

	ldmfd r0!, {ip, lr}
	msr spsr, ip

	msr cpsr_c, #0xDF /* System Mode */
	mov sp, r0
	pop {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, fp, ip, lr}
	msr cpsr_c, #0xD3 /* Supervisor Mode */

	movs pc, lr

.global kickIt
kickIt:
	/* Save current task state */
	stmfd r0!, {r4-r11, ip, lr}

	/* Load the new task */
	ldmfd r1!, {r4-r11, ip, lr}

	movs pc, lr /* Jump to where we left off */

.global spawn
spawn:
	/* Save current task state */
	push {r4, r5, r6, r7, r8, r9, r10, fp, ip, lr}
	mov r0, sp

	ldmfd r1!, {lr}

	movs pc, lr /* Jump to where we left off */

.global testSpawn
testSpawn:
	ldmfd r0!, {lr}

	movs pc, lr