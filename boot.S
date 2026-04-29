.code16
.section .text._start
.global _start
.extern mainC

_start:
	cli
	xorw %ax, %ax
	movw %ax, %ds
	lgdtl gdt_descriptor
	movl %cr0, %eax
	orl $1, %eax
	movl %eax, %cr0
	.byte 0x66, 0xea
	.long start32
	.word 0x08

.code32

start32:
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	movl $0x90000, %esp
	movl $0x1F211F21, 0xB8000
	call mainC
	hlt

gdt_start:
	.quad 0
gdt_code:
	.quad 0x00cf9a000000ffff
gdt_data:
	.quad 0x00cf92000000ffff
gdt_end:
gdt_descriptor:
	.word gdt_end - gdt_start - 1
	.long gdt_start
