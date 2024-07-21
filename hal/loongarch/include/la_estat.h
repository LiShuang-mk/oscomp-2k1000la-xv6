//
// Created by Li shuang ( pseudonym ) on 2024-04-15 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#ifndef _LA_ESTAT_H_
#define _LA_ESTAT_H_

#define _LA_ECODE_MAX_NUM_ 0x40 

enum _LA_ECODE_
{
	_int_ = 0x0,
	_pil_ = 0x1,
	_pis_ = 0x2,
	_pif_ = 0x3,
	_pme_ = 0x4,
	_pnr_ = 0x5,
	_pnx_ = 0x6,
	_ppi_ = 0x7,
	_ade_ = 0x8,
	_ale_ = 0x9,
	_bce_ = 0xA,
	_sys_ = 0xB,
	_brk_ = 0xC,
	_ine_ = 0xD,
	_ipe_ = 0xE,
	_fpd_ = 0xF,
	_sxd_ = 0x10,
	_asxd_ = 0x11,
	_fpe_ = 0x12,
	_wpe_ = 0x13,
	_btd_ = 0x14,
	_bte_ = 0x15,
	_gspr_ = 0x16,
	_hvc_ = 0x17,
	_gcc_ = 0x18, 
};

extern const char *_la_ecode_spec_[ _LA_ECODE_MAX_NUM_ ];
// extern void( *_la_ehandle_[ _LA_ECODE_MAX_NUM_ ] )( void );

#endif // la_estat.h 