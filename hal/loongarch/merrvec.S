//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

.section .text.merrentry
.globl _wrp_machine_trap
.globl handle_merr
.align 0x4
handle_merr:
    bl _wrp_machine_trap
    ertn