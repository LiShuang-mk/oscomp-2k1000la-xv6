//
// Created by Li shuang ( pseudonym ) on 2024-07-09
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#ifndef _LAREGS_H_
#define _LAREGS_H_ 

#define _LA_UINT64_ unsigned long long
#define _LA_CSR_MAX_NUM_ 0x200
#define _LA_R_CSR_TYPE_ static inline _LA_UINT64_ 
#define _LA_W_CSR_TYPE_ static inline void 

#define rFuncName(_CSR_NAME_) _r_##_CSR_NAME_##_
#define wFuncName(_CSR_NAME_) _w_##_CSR_NAME_##_

#define _CSR_ENUM_NAME_(_CSR_NAME_) _la_##_CSR_NAME_##_

#define _BUILD_LA_R_CSR_FUNC_(_CSR_NAME_) _LA_R_CSR_TYPE_ rFuncName(_CSR_NAME_)( void ) { \
	_LA_UINT64_ v; \
	asm volatile( "csrrd %0, %1" : "=r" ( v ) : "i" ( _CSR_ENUM_NAME_(_CSR_NAME_) ) ); \
	return v; \
}

#define _BUILD_LA_W_CSR_FUNC_(_CSR_NAME_) _LA_W_CSR_TYPE_ wFuncName(_CSR_NAME_)( _LA_UINT64_ _d_ ) { \
	asm volatile( "csrwr %0, %1" :: "r" ( _d_ ), "i" ( _CSR_ENUM_NAME_(_CSR_NAME_) ) ); \
}


// this file include some operations for operating the registers 
// main purpose is to operate csr registers 

enum _LA_CSR_
{
	_la_crmd_ = 0x0,
	_la_prmd_ = 0x1,
	_la_euen_ = 0x2,
	_la_misc_ = 0x3,
	_la_ecfg_ = 0x4,
	_la_estat_ = 0x5,
	_la_era_ = 0x6,
	_la_badv_ = 0x7,
	_la_badi_ = 0x8,
	_la_eentry_ = 0xc,
	_la_tlbidx_ = 0x10, 
	_la_tlbehi_ = 0x11,
	_la_tlbelo0_ = 0x12,
	_la_tlbelo1_ = 0x13,
	_la_asid_ = 0x18,
	_la_pgdl_ = 0x19,
	_la_pgdh_ = 0x1a,
	_la_pgd_ = 0x1b,
	_la_pwcl_ = 0x1c,
	_la_pwch_ = 0x1d,
	_la_stlbps_ = 0x1e,
	_la_rvacfg_ = 0x1f, 
	_la_cpuid_ = 0x20,
	_la_prcfg1_ = 0x21,
	_la_prcfg2_ = 0x22,
	_la_prcfg3_ = 0x23,
	_la_save0_ = 0x30,
	_la_tid_ = 0x40, 
	_la_tcfg_ = 0x41,
	_la_tval_ = 0x42,
	_la_cntc_ = 0x43,
	_la_ticlr_ = 0x44,
	_la_llbctl_ = 0x60,
	_la_impctl1_ = 0x80,
	_la_impctl2_ = 0x81, 
	_la_tlbrentry_ = 0x88,
	_la_tlbrbadv_ = 0x89,
	_la_tlbrera_ = 0x8a, 
	_la_tlbrsave_ = 0x8b,
	_la_tlbrelo0_ = 0x8c,
	_la_tlbrelo1_ = 0x8d, 
	_la_tlbrehi_ = 0x8e,
	_la_tlbrprmd_ = 0x8f,
	_la_merrctl_ = 0x90,
	_la_merrinfo1_ = 0x91,
	_la_merrinfo2_ = 0x92,
	_la_merrentry_ = 0x93,
	_la_merrera_ = 0x94,
	_la_merrsave_ = 0x95,
	_la_ctag_ = 0x98,
	_la_msgis0_ = 0xa0, 
	_la_msgis1_ = 0xa1, 
	_la_msgis2_ = 0xa2, 
	_la_msgis3_ = 0xa3,
	_la_msgir_ = 0xa4,
	_la_msgie_ = 0xa5, 
	_la_dmwin0_ = 0x180,
	_la_dmwin1_ = 0x181,
	_la_dmwin2_ = 0x182,
	_la_dmwin3_ = 0x183,
};

extern _LA_UINT64_( *_la_r_csr_[ _LA_CSR_MAX_NUM_ ] )( void );
extern void( *_la_w_csr_[ _LA_CSR_MAX_NUM_ ] )( _LA_UINT64_ );

#endif // laregs.h