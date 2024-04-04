//
// Created by Li shuang ( pseudonym ) on 2024-04-04 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hal/laregs.h"

_LA_R_CSR_TYPE_
rcrmd( void )
{
	_LA_UINT64_ v;
	asm volatile( "csrrd %0, %1" : "=r" ( v ) : "i" ( _LA_CRMD_ ) );
	return v;
}

_LA_R_CSR_TYPE_
rpgdl( void )
{
	_LA_UINT64_ v;
	asm volatile( "csrrd %0, %1" : "=r" ( v ) : "i" ( _LA_PGDL_ ) );
	return v;
}

_LA_UINT64_( *_la_r_csr_[ _LA_CSR_MAX_NUM_ ] )( void ) = {
	[_LA_CRMD_] = rcrmd,
	[ _LA_PGDL_ ] =  	rpgdl
};
