/ * Multiboot header constants * /
.set ALIGN,     1<<0
.set MEMINFO,   1<<1
.set VIDEOINFO, 1<<2
.set MFLAGS,     ALIGN | MEMINFO | VIDEOINFO
.set MAGIC,     0x1BADB002
.set CHECKSUM,  -(MAGIC + MFLAGS)
.set VIDEOMODE,  0
.set VWIDTH,     1024
.set VHEIGHT,    768
.set VDEPTH,     32

/ * FPU FLAGS * /
.set CR0_EM,     1<<2
.set CR0_TS,     1<<3
.set FFLAGS,     CR0_EM | CR0_TS

/ * Create multiboot header * /
.align 4
.long MAGIC
.long MFLAGS
.long CHECKSUM
.long 0, 0, 0, 0, 0
.long VIDEOMODE
.long VWIDTH
.long VHEIGHT
.long VDEPTH

/ * Multiboot does not define a stack, and so this allocates a small one of 16 KiB * /
.section .bss
.globl _multiboot_info_start
_multiboot_info_start:
    .long 0

.globl _multiboot_magic
_multiboot_magic:
    .long 0

.globl _heap_start
_heap_start:
    .long 0

.globl _heap_end
_heap_end:
    .long 0

.align 32
gdt:
.skip 48
gdt_end:

.align 16
stack_bottom:
.skip 16384
stack_top:

.align 32
heap_bottom:
.skip 16384
heap_top:

.section .data
.globl _fpu_test
_fpu_test:
    .long 0x55AA

/ * _start is the entry points * /
.section .text
.global _start
.type _start, @function
_start:
    / * bootloader has loaded into 32-bit protected mode on an x86 machine. Interrupts are disabled, * /
    / * paging is diabled, and kernel has full control of the CPU. * /
    
    / * stack is setup by moving esp (top-of-stack pointer) to stack_top. This is done in assembly as C * /
    / * cannot function without a stack. * /
    mov $stack_top, %esp

    / * Cruicial processor state should be initialised here before the full kernel starts. This means * /
    / * floats, GDT, paging and some c++ features will not work untill they are implemented here. * /
    movl %eax, _multiboot_magic
    movl %ebx, _multiboot_info_start

    movl $heap_bottom, _heap_start
    movl $heap_top, _heap_end

    mov %cr0, %eax
    mov 0, %ebx
    or  FFLAGS, %ebx
    neg %ebx
    and %ebx, %eax
    mov %eax, %cr0
    FNINIT
    FNSTSW _fpu_test

    / * Enter the high-level kernel * /
    call kernel_main

    / * Put the computer into an infinite loop. Clear interrupts, halt, then re-halt if it wakes up * /
    cli
    1:
    hlt
    jmp 1b

/ * Set the size of the _start symbol to the current location ('.'), minus its start. * /
.size _start, . - _start
   