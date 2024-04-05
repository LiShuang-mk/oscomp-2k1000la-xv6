//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hal/asm.h"

.section tlbrentry
.globl handle_tlbr
.align 0x4
handle_tlbr:
	csrwr	$t0, LOONGARCH_CSR_TLBRSAVE
	csrrd	$t0, LOONGARCH_CSR_PGD
//	lddir	$t0, $t0, 4
//	addi.d  $t0, $t0, -1
//	lu52i.d	$t0, $t0, -1792
	lu52i.d	$t0, $t0, 0x0
	lddir	$t0, $t0, 3
	srli.d	$t0, $t0, 12 
	slli.d	$t0, $t0, 12
	lddir	$t0, $t0, 2
	srli.d	$t0, $t0, 12 
	slli.d	$t0, $t0, 12
//	lu52i.d	$t0, $t0, -1792
//	addi.d  $t0, $t0, -1
	lddir	$t0, $t0, 1
	srli.d	$t0, $t0, 12 
	slli.d	$t0, $t0, 12
//	addi.d  $t0, $t0, -1
//	lu52i.d	$t0, $t0, -1792
//	lddir   $t0, $t0, 1
//	lu52i.d	$t0, $t0, -1792
//	addi.d  $t0, $t0, -1
	ldpte	$t0, 0
	ldpte	$t0, 1
	csrrd	$t0, 0x8c 
	csrrd	$t0, 0x8d 
	tlbfill
	csrrd	$t0, LOONGARCH_CSR_TLBRSAVE
	ertn
	
