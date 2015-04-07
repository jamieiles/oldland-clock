_irq_vector:
	sub	$sp, $sp, 128
	str32	$r0,  [$sp, 0x00]
	str32	$r1,  [$sp, 0x04]
	str32	$r2,  [$sp, 0x08]
	str32	$r3,  [$sp, 0x0c]
	str32	$r4,  [$sp, 0x10]
	str32	$r5,  [$sp, 0x14]
	str32	$r6,  [$sp, 0x18]
	str32	$r7,  [$sp, 0x1c]
	str32	$r8,  [$sp, 0x20]
	str32	$r9,  [$sp, 0x24]
	str32	$r10, [$sp, 0x28]
	str32	$r11, [$sp, 0x2c]
	str32	$r12, [$sp, 0x30]
	str32	$fp,  [$sp, 0x34]
	str32	$sp,  [$sp, 0x38]
	str32	$lr,  [$sp, 0x3c]
	gcr	$r0, 1
	str32	$r0,  [$sp, 0x40]
	gcr	$r0, 2
	str32	$r0,  [$sp, 0x44]
	gcr	$r0, 3
	str32	$r0,  [$sp, 0x48]

	sub	$sp, $sp, 16
	call	handle_irq
	add	$sp, $sp, 16

	ldr32	$r0,  [$sp, 0x48]
	scr	3, $r0
	ldr32	$r0,  [$sp, 0x44]
	scr	2, $r0
	ldr32	$r0,  [$sp, 0x40]
	scr	1, $r0
	ldr32	$lr,  [$sp, 0x3c]
	ldr32	$fp,  [$sp, 0x34]
	ldr32	$r12, [$sp, 0x30]
	ldr32	$r11, [$sp, 0x2c]
	ldr32	$r10, [$sp, 0x28]
	ldr32	$r9,  [$sp, 0x24]
	ldr32	$r8,  [$sp, 0x20]
	ldr32	$r7,  [$sp, 0x1c]
	ldr32	$r6,  [$sp, 0x18]
	ldr32	$r5,  [$sp, 0x14]
	ldr32	$r4,  [$sp, 0x10]
	ldr32	$r3,  [$sp, 0x0c]
	ldr32	$r2,  [$sp, 0x08]
	ldr32	$r1,  [$sp, 0x04]
	ldr32	$r0,  [$sp, 0x00]
	add	$sp, $sp, 128

	rfe

_bad_vector:
	call	bad_vector
1:
	b	1b

	.globl init_vectors
init_vectors:
	movhi	$r0, %hi(_ex_table)
	orlo	$r0, $r0, %lo(_ex_table)
	scr	0, $r0

	mov	$r0, 0
	or	$r0, $r0, 0x70 /* ICACHE | DCACHE | IRQ */
	scr	1, $r0

	ret

	.section ".rodata"
	.balign 64
_ex_table:
	b	_bad_vector	/* Reset */
	b	_bad_vector	/* Illegal instruction */
	b	_bad_vector	/* Software interrupt */
	b	_irq_vector	/* IRQ */
	b	_bad_vector	/* Instruction fetch abort */
	b	_bad_vector	/* Data abort */
