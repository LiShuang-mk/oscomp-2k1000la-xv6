//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "asm.h"

.section .text.tlbrentry
.globl handle_tlbr
.align 0x4
handle_tlbr:
	csrwr	$t0, LOONGARCH_CSR_TLBRSAVE
	csrrd	$t0, 0x5	# read estat
	csrrd	$t0, LOONGARCH_CSR_PGD
	lddir	$t0, $t0, 3
	srli.d	$t0, $t0, 12 
	slli.d	$t0, $t0, 12
	lddir	$t0, $t0, 2
	srli.d	$t0, $t0, 12 
	slli.d	$t0, $t0, 12
	lddir	$t0, $t0, 1
	srli.d	$t0, $t0, 12 
	slli.d	$t0, $t0, 12
	ldpte	$t0, 0
	ldpte	$t0, 1
	csrrd	$t0, 0x8c 
	csrrd	$t0, 0x8d 
	tlbfill
	csrrd	$t0, LOONGARCH_CSR_TLBRSAVE
	ertn
	
