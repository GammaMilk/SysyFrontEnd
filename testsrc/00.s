	.text
	.syntax unified
	.fpu	neon
	.file	"00_main.sy"
	.globl	main                            @ -- Begin function main
	.p2align	2
	.type	main,%function
	.code	32                              @ @main
main:
	.fnstart
@ %bb.0:
	.pad	#4
	sub	sp, sp, #4
	movw	r0, #0
	str	r0, [sp]
	movw	r0, #3
	add	sp, sp, #4
	bx	lr
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cantunwind
	.fnend
                                        @ -- End function
	.ident	"(built by Brecht Sanders) clang version 16.0.2"
	.section	".note.GNU-stack","",%progbits
	.eabi_attribute	30, 6	@ Tag_ABI_optimization_goals