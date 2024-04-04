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

// this file include some operations for operating the registers 
// main purpose is to operate csr registers 

enum _LA_CSR_
{
	_LA_CRMD_ = 0x0,
	_LA_CPUID_ = 0x20,
	_LA_SAVE0_ = 0x30,
	_LA_DMWIN0_ = 0x180,
	_LA_DMWIN1_ = 0x181,
	_LA_DMWIN2_ = 0x182,
	_LA_DMWIN3_ = 0x183,
	_LA_TLBEHI = 0x11,
	_LA_PGDL_ = 0x19,
	_LA_PGD_ = 0x1b,
	_LA_TLBRENTRY_ = 0x88,
	_LA_TLBRBADV_ = 0x89,
	_LA_TLBRSAVE_ = 0x8b,
};

extern _LA_UINT64_( *_la_r_csr_[ _LA_CSR_MAX_NUM_ ] )( void );
extern void( *_la_w_csr_[ _LA_CSR_MAX_NUM_ ] )( _LA_UINT64_ ); 

#endif // laregs.h