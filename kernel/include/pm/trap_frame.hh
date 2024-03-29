//
// Created by Li shuang ( pseudonym ) on 2024-03-29 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"

struct TrapFrame
{
	/*   0 */ uint64 ra;
	/*   8 */ uint64 tp;
	/*  16 */ uint64 sp;
	/*  24 */ uint64 a0;
	/*  32 */ uint64 a1;
	/*  40 */ uint64 a2;
	/*  48 */ uint64 a3;
	/*  56 */ uint64 a4;
	/*  64 */ uint64 a5;
	/*  72 */ uint64 a6;
	/*  80 */ uint64 a7;
	/*  88 */ uint64 t0;
	/*  96 */ uint64 t1;
	/* 104 */ uint64 t2;
	/* 112 */ uint64 t3;
	/* 120 */ uint64 t4;
	/* 128 */ uint64 t5;
	/* 136 */ uint64 t6;
	/* 144 */ uint64 t7;
	/* 152 */ uint64 t8;
	/* 160 */ uint64 r21;
	/* 168 */ uint64 fp;
	/* 176 */ uint64 s0;
	/* 184 */ uint64 s1;
	/* 192 */ uint64 s2;
	/* 200 */ uint64 s3;
	/* 208 */ uint64 s4;
	/* 216 */ uint64 s5;
	/* 224 */ uint64 s6;
	/* 232 */ uint64 s7;
	/* 240 */ uint64 s8;
	/* 248 */ uint64 kernel_sp;     // top of process's kernel stack
	/* 256 */ uint64 kernel_trap;   // usertrap()
	/* 264 */ uint64 era;           // saved user program counter
	/* 272 */ uint64 kernel_hartid; // saved kernel tp
	/* 280 */ uint64 kernel_pgdl;   // saved kernel pagetable
};