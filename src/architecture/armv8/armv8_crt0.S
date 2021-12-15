// EPOS ARMv7 Program Starter

        .file "armv8_crt0.S"

        .align  2
        .global _start
        .type   _start, function
_start:

#ifdef __cortex_53__
        // Allocate a temporary stack for INIT, since we don't have a proper SETUP for Cortex-M
        ldr     x0, =__boot_stack__
        mov     sp, x0

        // Clear the BSS
        bl      _clear_bss
#endif

        bl      _init

        .align  2
        .globl  __epos_app_entry
        .type   __epos_app_entry, function
__epos_app_entry:
        // Main's stack was allocated by Thread::init()
        bl      main
        str     x0, [sp, #-16]!        // save main's return value to be used by exit()
        bl      _fini
        bl      _exit
