//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

.section merrentry
.globl machine_trap
.globl handle_merr
.align 0x4
handle_merr:
    bl machine_trap
    ertn