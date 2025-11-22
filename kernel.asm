section .text
default rel
bits 64
global asm_kernel
asm_kernel:
	; rcx = n, rdx = &a, r8 = &b
	xor rax, rax
	cvtsi2ss xmm0, rax
loop:
	dec rcx
	movss xmm1, [rdx + rcx * 4]
	movss xmm2, [r8 + rcx * 4]
	mulss xmm1, xmm2
	addss xmm0, xmm1
	cmp rcx, 0
	jne loop
end_loop:
	ret
