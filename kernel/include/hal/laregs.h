//
// Created by Li shuang ( pseudonym ) on 2024-04-04 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#ifndef _LAREGS_H_
#define _LAREGS_H_ 

#define _LA_UINT64_ unsigned long 
#define _LA_CSR_MAX_NUM_ 0x1000
#define _LA_R_CSR_TYPE_ static inline _LA_UINT64_ 
#define _LA_W_CSR_TYPE_ static inline void 

#define rFuncName(_CSR_NAME_) _r_##_CSR_NAME_##_
#define wFuncName(_CSR_NAME_) _w_##_CSR_NAME_##_

#define _BUILD_LA_R_CSR_FUNC_(_CSR_NAME_) _LA_R_CSR_TYPE_ rFuncName(_CSR_NAME_)( void ) { \
	_LA_UINT64_ v; \
	asm volatile( "csrrd %0, %1" : "=r" ( v ) : "i" ( _la_##_CSR_NAME_##_ ) ); \
	return v; \
}

#define _BUILD_LA_W_CSR_FUNC_(_CSR_NAME_) _LA_W_CSR_TYPE_ wFuncName(_CSR_NAME_)( _LA_UINT64_ _d_ ) { \
	asm volatile( "csrwr %0, %1" :: "r" ( _d_ ), "i" ( _la_##_CSR_NAME_##_ ) ); \
}


// this file include some operations for operating the registers 
// main purpose is to operate csr registers 

enum _LA_CSR_
{
	_la_crmd_ = 0x0,
	_la_tlbehi_ = 0x11,
	_la_asid_ = 0x18,
	_la_pgdl_ = 0x19,
	_la_pgdh_ = 0x1a,
	_la_pgd_ = 0x1b,
	_la_pwcl_ = 0x1c,
	_la_pwch_ = 0x1d,
	_la_stlbps_ = 0x1e,
	_la_cpuid_ = 0x20,
	_la_save0_ = 0x30,
	_la_tlbrentry_ = 0x88,
	_la_tlbrbadv_ = 0x89,
	_la_tlbrsave_ = 0x8b,
	_la_tlbrehi_ = 0x8e, 
	_la_dmwin0_ = 0x180,
	_la_dmwin1_ = 0x181,
	_la_dmwin2_ = 0x182,
	_la_dmwin3_ = 0x183,
};

extern _LA_UINT64_( *_la_r_csr_[ _LA_CSR_MAX_NUM_ ] )( void );
extern void( *_la_w_csr_[ _LA_CSR_MAX_NUM_ ] )( _LA_UINT64_ );

#endif // laregs.h