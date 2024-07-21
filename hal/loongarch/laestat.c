//
// Created by Li shuang ( pseudonym ) on 2024-04-15 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "la_estat.h"

#define bspec(code, spec) [_##code##_]	#spec,
const char *_la_ecode_spec_[ 0x40 ] =
{
	bspec( int, 中断 )
	bspec( pil, load操作页无效例外 )
	bspec( pis, store操作页无效例外 )
	bspec( pif, 取指操作页无效例外 )
	bspec( pme, 页修改例外 )
	bspec( pnr, 页不可读例外 )
	bspec( pnx, 页不可执行例外 )
	bspec( ppi, 页特权等级不合规例外 )
	bspec( ade, 访存地址错例外 )
	bspec( ale, 地址非对齐例外 )
	bspec( bce, 边界检查错例外 )
	bspec( sys, 系统调用例外 )
	bspec( brk, 断点例外 )
	bspec( ine, 指令不存在例外 )
	bspec( ipe, 指令特权等级错例外 )
	bspec( fpd, 浮点指令未使能例外 )
	bspec( sxd, 128位向量扩展指令未使能例外 )
	bspec( asxd, 256位向量扩展指令未使能例外 )
	bspec( fpe, 浮点指令例外 )
	bspec( wpe, 监测点例外 )
	bspec( btd, 二进制翻译扩展指令未使能例外 )
	bspec( bte, 二进制翻译相关例外 )
	bspec( gspr, 客户机敏感特权资源例外 )
	bspec( hvc, 虚拟机监控调用例外 )
	bspec( gcc, 客户机CSR修改例外 )
};
#undef bspec 