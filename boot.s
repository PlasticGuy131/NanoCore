/ * Multiboot header constants * /
.set ALIGN,     1<<0
.set MEMINFO,   1<<1
.set VIDEOINFO, 1<<2
.set FLAGS,     ALIGN | MEMINFO  //| VIDEOINFO
.set MAGIC,     0x1BADB002
.set CHECKSUM,  -(MAGIC + FLAGS)
.set VIDEOMODE,  0
.set VWIDTH,     1024
.set VHEIGHT,    768
.set VDEPTH,     32

/ * Create multiboot header * /
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
/*.long 0, 0, 0, 0, 0
.long VIDEOMODE
.long VWIDTH
.long VHEIGHT
.long VDEPTH*/

.extern multiboot_info

/ * Multiboot does not define a stack, and so this allocates a small one of 16 KiB * /
.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

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
    movl %ebx, multiboot_info_start

    / * Enter the high-level kernel * /
    call kernel_main

    / * Put the computer into an infinite loop. Clear interrupts, halt, then re-halt if it wakes up * /
    cli
    1:
    hlt
    jmp 1b

/ * Set the size of the _start symbol to the current location ('.'), minus its start. * /
.size _start, . - _start
   