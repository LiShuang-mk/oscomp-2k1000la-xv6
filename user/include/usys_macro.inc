//
// Created by Li shuang ( pseudonym ) on 2024-05-21 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

.macro GEN_USER_SYSCALL sys_name sys_num
	.global \sys_name
	\sys_name :
	li.d	$a7, \sys_num
	syscall	0
	jirl	$zero, $ra, 0
.endm