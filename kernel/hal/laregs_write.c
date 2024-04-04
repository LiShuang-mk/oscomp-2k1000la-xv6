//
// Created by Li shuang ( pseudonym ) on 2024-04-04 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hal/laregs.h"

_LA_W_CSR_TYPE_
_wcrmd_( _LA_UINT64_ _d_ )
{
	asm volatile( "csrwr %0, %1" :: "r" ( _d_ ), "i" ( _LA_CRMD_ ) );
}

_LA_W_CSR_TYPE_
_wpgdl_( _LA_UINT64_ _d_ )
{
	asm volatile( "csrrd %0, %1" :: "r" ( _d_ ), "i" ( _LA_PGDL_ ) );
}

void( *_la_w_csr_[ _LA_CSR_MAX_NUM_ ] )( _LA_UINT64_ ) = {
	[_LA_CRMD_] _wcrmd_,
	[ _LA_PGDL_ ]		_wpgdl_
};