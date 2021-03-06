/*
 *  linux/arch/arm/lib/io-readsb.S
 *
 *  Copyright (C) 1995-2000 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/linkage.h>
#include <asm/assembler.h>
#include <linux/config.h>

.insb_align:	rsb	ip, ip, #4
		cmp	ip, r2
		movgt	ip, r2
		cmp	ip, #2
		ldrb	r3, [r0]
#ifdef CONFIG_NDS_ROM8BIT
		swpb	r3, r3, [r1]
		add	r1, r1, #1
#else
		strb	r3, [r1], #1
#endif
		ldrgeb	r3, [r0]
#ifdef CONFIG_NDS_ROM8BIT
		swpgeb	r3, r3, [r1]
		addge	r1, r1, #1
#else
		strgeb	r3, [r1], #1
#endif
		ldrgtb	r3, [r0]
#ifdef CONFIG_NDS_ROM8BIT
		swpgtb	r3, r3, [r1]
		addgt	r1, r1, #1
#else
		strgtb	r3, [r1], #1
#endif
		subs	r2, r2, ip
		bne	.insb_aligned

ENTRY(__raw_readsb)
		teq	r2, #0		@ do we have to check for the zero len?
		moveq	pc, lr
		ands	ip, r1, #3
		bne	.insb_align

.insb_aligned:	stmfd	sp!, {r4 - r6, lr}

		subs	r2, r2, #16
		bmi	.insb_no_16

.insb_16_lp:	ldrb	r3, [r0]
		ldrb	r4, [r0]
		ldrb	r5, [r0]
		mov	r3, r3,     put_byte_0
		ldrb	r6, [r0]
		orr	r3, r3, r4, put_byte_1
		ldrb	r4, [r0]
		orr	r3, r3, r5, put_byte_2
		ldrb	r5, [r0]
		orr	r3, r3, r6, put_byte_3
		ldrb	r6, [r0]
		mov	r4, r4,     put_byte_0
		ldrb	ip, [r0]
		orr	r4, r4, r5, put_byte_1
		ldrb	r5, [r0]
		orr	r4, r4, r6, put_byte_2
		ldrb	r6, [r0]
		orr	r4, r4, ip, put_byte_3
		ldrb	ip, [r0]
		mov	r5, r5,     put_byte_0
		ldrb	lr, [r0]
		orr	r5, r5, r6, put_byte_1
		ldrb	r6, [r0]
		orr	r5, r5, ip, put_byte_2
		ldrb	ip, [r0]
		orr	r5, r5, lr, put_byte_3
		ldrb	lr, [r0]
		mov	r6, r6,     put_byte_0
		orr	r6, r6, ip, put_byte_1
		ldrb	ip, [r0]
		orr	r6, r6, lr, put_byte_2
		orr	r6, r6, ip, put_byte_3
		stmia	r1!, {r3 - r6}

		subs	r2, r2, #16
		bpl	.insb_16_lp

		tst	r2, #15
		LOADREGS(eqfd, sp!, {r4 - r6, pc})

.insb_no_16:	tst	r2, #8
		beq	.insb_no_8

		ldrb	r3, [r0]
		ldrb	r4, [r0]
		ldrb	r5, [r0]
		mov	r3, r3,     put_byte_0
		ldrb	r6, [r0]
		orr	r3, r3, r4, put_byte_1
		ldrb	r4, [r0]
		orr	r3, r3, r5, put_byte_2
		ldrb	r5, [r0]
		orr	r3, r3, r6, put_byte_3
		ldrb	r6, [r0]
		mov	r4, r4,     put_byte_0
		ldrb	ip, [r0]
		orr	r4, r4, r5, put_byte_1
		orr	r4, r4, r6, put_byte_2
		orr	r4, r4, ip, put_byte_3
		stmia	r1!, {r3, r4}

.insb_no_8:	tst	r2, #4
		beq	.insb_no_4

		ldrb	r3, [r0]
		ldrb	r4, [r0]
		ldrb	r5, [r0]
		ldrb	r6, [r0]
		mov	r3, r3,     put_byte_0
		orr	r3, r3, r4, put_byte_1
		orr	r3, r3, r5, put_byte_2
		orr	r3, r3, r6, put_byte_3
		str	r3, [r1], #4

.insb_no_4:	ands	r2, r2, #3
		LOADREGS(eqfd, sp!, {r4 - r6, pc})

		cmp	r2, #2
		ldrb	r3, [r0]
#ifdef CONFIG_NDS_ROM8BIT
		swpb	r3, r3, [r1]
		add	r1, r1, #1
#else
		strb	r3, [r1], #1
#endif
		ldrgeb	r3, [r0]
#ifdef CONFIG_NDS_ROM8BIT
		swpgeb	r3, r3, [r1]
		addge	r1, r1, #1	
#else
		strgeb	r3, [r1], #1
#endif
		ldrgtb	r3, [r0]
#ifdef CONFIG_NDS_ROM8BIT
		swpgtb	r3, r3, [r1]
#else
		strgtb	r3, [r1]
#endif
		LOADREGS(fd, sp!, {r4 - r6, pc})
