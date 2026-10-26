=== NMI @ 0x80085c0 ===
80085c0:	eb03 03c2 	add.w	r3, r3, r2, lsl #3
80085c4:	e9d3 2300 	ldrd	r2, r3, [r3]
80085c8:	f000 f9c4 	bl	0x8008954
80085cc:	900e      	str	r0, [sp, #56]	@ 0x38
80085ce:	9107      	str	r1, [sp, #28]
80085d0:	9801      	ldr	r0, [sp, #4]
80085d2:	f000 fc02 	bl	0x8008dda
80085d6:	9105      	str	r1, [sp, #20]
80085d8:	e9dd 1306 	ldrd	r1, r3, [sp, #24]
80085dc:	4682      	mov	sl, r0
80085de:	9a0e      	ldr	r2, [sp, #56]	@ 0x38
80085e0:	9804      	ldr	r0, [sp, #16]
80085e2:	f000 f9b7 	bl	0x8008954
80085e6:	4a7c      	ldr	r2, [pc, #496]	@ (0x80087d8)
80085e8:	447a      	add	r2, pc
80085ea:	9b0d      	ldr	r3, [sp, #52]	@ 0x34

=== HardFault @ 0x8007fda ===
(no match)

=== MemMgr @ 0x80085bc ===
80085bc:	447b      	add	r3, pc
80085be:	9a0d      	ldr	r2, [sp, #52]	@ 0x34
80085c0:	eb03 03c2 	add.w	r3, r3, r2, lsl #3
80085c4:	e9d3 2300 	ldrd	r2, r3, [r3]
80085c8:	f000 f9c4 	bl	0x8008954
80085cc:	900e      	str	r0, [sp, #56]	@ 0x38
80085ce:	9107      	str	r1, [sp, #28]
80085d0:	9801      	ldr	r0, [sp, #4]
80085d2:	f000 fc02 	bl	0x8008dda
80085d6:	9105      	str	r1, [sp, #20]
80085d8:	e9dd 1306 	ldrd	r1, r3, [sp, #24]
80085dc:	4682      	mov	sl, r0
80085de:	9a0e      	ldr	r2, [sp, #56]	@ 0x38
80085e0:	9804      	ldr	r0, [sp, #16]
80085e2:	f000 f9b7 	bl	0x8008954
80085e6:	4a7c      	ldr	r2, [pc, #496]	@ (0x80087d8)

=== BusFault @ 0x80046c4 ===
80046c4:	0040      	lsls	r0, r0, #1
80046c6:	0000      	movs	r0, r0
80046c8:	7000      	strb	r0, [r0, #0]
80046ca:	8888      	ldrh	r0, [r1, #4]
80046cc:	2010      	movs	r0, #16
80046ce:	0020      	movs	r0, r4
80046d0:	0020      	movs	r0, r4
80046d2:	0000      	movs	r0, r0
80046d4:	3800      	subs	r0, #0
80046d6:	9444      	str	r4, [sp, #272]	@ 0x110
80046d8:	b4b4      	push	{r2, r4, r5, r7}
80046da:	44b8      	add	r8, r7
80046dc:	0038      	movs	r0, r7
80046de:	0000      	movs	r0, r0
80046e0:	2000      	movs	r0, #0
80046e2:	3020      	adds	r0, #32

=== UsageFlt @ 0x8008bb2 ===
(no match)

=== SysTick @ 0x80085c4 ===
80085c4:	e9d3 2300 	ldrd	r2, r3, [r3]
80085c8:	f000 f9c4 	bl	0x8008954
80085cc:	900e      	str	r0, [sp, #56]	@ 0x38
80085ce:	9107      	str	r1, [sp, #28]
80085d0:	9801      	ldr	r0, [sp, #4]
80085d2:	f000 fc02 	bl	0x8008dda
80085d6:	9105      	str	r1, [sp, #20]
80085d8:	e9dd 1306 	ldrd	r1, r3, [sp, #24]
80085dc:	4682      	mov	sl, r0
80085de:	9a0e      	ldr	r2, [sp, #56]	@ 0x38
80085e0:	9804      	ldr	r0, [sp, #16]
80085e2:	f000 f9b7 	bl	0x8008954
80085e6:	4a7c      	ldr	r2, [pc, #496]	@ (0x80087d8)
80085e8:	447a      	add	r2, pc
80085ea:	9b0d      	ldr	r3, [sp, #52]	@ 0x34
80085ec:	eb02 03c3 	add.w	r3, r2, r3, lsl #3

=== ExtInt4 @ 0x8007740 ===
(no match)

=== ExtInt5 @ 0x80085c2 ===
(no match)

=== ExtInt6 @ 0x8008bb0 ===
8008bb0:	f116 0780 	adds.w	r7, r6, #128	@ 0x80
8008bb4:	f145 0500 	adc.w	r5, r5, #0
8008bb8:	ea4f 2717 	mov.w	r7, r7, lsr #8
8008bbc:	ea4f 6606 	mov.w	r6, r6, lsl #24
8008bc0:	ea47 6705 	orr.w	r7, r7, r5, lsl #24
8008bc4:	f1a6 46de 	sub.w	r6, r6, #1862270976	@ 0x6f000000
8008bc8:	f1b6 5f80 	cmp.w	r6, #268435456	@ 0x10000000
8008bcc:	ea4f 2515 	mov.w	r5, r5, lsr #8
8008bd0:	d91f      	bls.n	0x8008c12
8008bd2:	4638      	mov	r0, r7
8008bd4:	f024 0c01 	bic.w	ip, r4, #1
8008bd8:	eb05 71c4 	add.w	r1, r5, r4, lsl #31
8008bdc:	f1bc 6ffe 	cmp.w	ip, #133169152	@ 0x7f00000
8008be0:	eb01 110c 	add.w	r1, r1, ip, lsl #4
8008be4:	d802      	bhi.n	0x8008bec
8008be6:	bcf0      	pop	{r4, r5, r6, r7}
