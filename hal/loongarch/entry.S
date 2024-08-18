#include "asm.h"

.section .text.osstartup
.global _entry
.global __init_atexit_func_entry
.global _cpu_init
_entry:
        la.global   $t0, 0f
        jirl        $zero, $t0, 0
0:      li.d        $t0, 0x9000000000000011   # CA, PLV0, 0x9000 xxxx xxxx xxxx
        csrwr       $t0, LOONGARCH_CSR_DMWIN0 
        li.d        $t0, 0x8000000000000001	  # UA, PLV0, 0x8000 xxxx xxxx xxxx   
        csrwr       $t0, LOONGARCH_CSR_DMWIN1
        li.d        $t0, 0x0000000000000000 
        csrwr       $t0, LOONGARCH_CSR_DMWIN2
        csrwr       $t0, LOONGARCH_CSR_DMWIN3
        csrwr       $t0, LOONGARCH_CSR_TLBRENTRY	
        li.w	    $t0, 0xb0		      # PLV=0, IE=0, PG=1
		csrwr	    $t0, LOONGARCH_CSR_CRMD
        invtlb      0x0,$zero,$zero

	# set up a stack for C.
        # entry_stack is declared in start.c,
        # with a 4096-byte stack per CPU.
        # sp = entry_stack + (hartid * 4096)
     
        la          $sp, entry_stack
        li.d        $a0, LOONGARCH_ENTRY_STACK_SIZE
		csrrd       $a1, LOONGARCH_CSR_CPUID
        addi.d      $tp, $a1, 0
        addi.d      $a1, $a1, 1
        mul.d       $a0, $a0, $a1
        add.d       $sp, $sp, $a0

		bnez        $tp, _skip_clear_bss
		la          $t0, _bss_start_addr
		la          $t1, _bss_end_addr
		li.d        $t2, 0
_loop_clear_bss:
		bgeu        $t0, $t1, _skip_clear_bss
		st.d        $t2, $t0, 0
		addi.d      $t0, $t0, 8
		b           _loop_clear_bss
_skip_clear_bss:

_test_init_atexit:
		# bl			__init_atexit_func_entry

		bl			_cpu_init

		# add.d       $t0, $zero, $t0
		# bl          -4

		li.d		$fp, 0

	# jump to xn6_start_kernel()
        bl          xn6_start_kernel
spin:
        b           spin
