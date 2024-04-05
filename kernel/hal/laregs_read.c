//
// Created by Li shuang ( pseudonym ) on 2024-04-04 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hal/laregs.h"

_BUILD_LA_R_CSR_FUNC_( crmd )
_BUILD_LA_R_CSR_FUNC_( prmd )
_BUILD_LA_R_CSR_FUNC_( euen )
_BUILD_LA_R_CSR_FUNC_( misc )
_BUILD_LA_R_CSR_FUNC_( ecfg )
_BUILD_LA_R_CSR_FUNC_( estat )
_BUILD_LA_R_CSR_FUNC_( era )
_BUILD_LA_R_CSR_FUNC_( badv )
_BUILD_LA_R_CSR_FUNC_( badi )
_BUILD_LA_R_CSR_FUNC_( eentry )
_BUILD_LA_R_CSR_FUNC_( tlbidx )
_BUILD_LA_R_CSR_FUNC_( tlbehi )
_BUILD_LA_R_CSR_FUNC_( tlbelo0 )
_BUILD_LA_R_CSR_FUNC_( tlbelo1 )
_BUILD_LA_R_CSR_FUNC_( asid )
_BUILD_LA_R_CSR_FUNC_( pgdl )
_BUILD_LA_R_CSR_FUNC_( pgdh )
_BUILD_LA_R_CSR_FUNC_( pgd )
_BUILD_LA_R_CSR_FUNC_( pwcl )
_BUILD_LA_R_CSR_FUNC_( pwch )
_BUILD_LA_R_CSR_FUNC_( stlbps )
_BUILD_LA_R_CSR_FUNC_( rvacfg )
_BUILD_LA_R_CSR_FUNC_( cpuid )
_BUILD_LA_R_CSR_FUNC_( prcfg1 )
_BUILD_LA_R_CSR_FUNC_( prcfg2 )
_BUILD_LA_R_CSR_FUNC_( prcfg3 )
_BUILD_LA_R_CSR_FUNC_( save0 )
_BUILD_LA_R_CSR_FUNC_( tid )
_BUILD_LA_R_CSR_FUNC_( tcfg )
_BUILD_LA_R_CSR_FUNC_( tval )
_BUILD_LA_R_CSR_FUNC_( cntc )
_BUILD_LA_R_CSR_FUNC_( ticlr )
_BUILD_LA_R_CSR_FUNC_( llbctl )
_BUILD_LA_R_CSR_FUNC_( impctl1 )
_BUILD_LA_R_CSR_FUNC_( impctl2 )
_BUILD_LA_R_CSR_FUNC_( tlbrentry )
_BUILD_LA_R_CSR_FUNC_( tlbrbadv )
_BUILD_LA_R_CSR_FUNC_( tlbrera )
_BUILD_LA_R_CSR_FUNC_( tlbrsave )
_BUILD_LA_R_CSR_FUNC_( tlbrelo0 )
_BUILD_LA_R_CSR_FUNC_( tlbrelo1 )
_BUILD_LA_R_CSR_FUNC_( tlbrehi )
_BUILD_LA_R_CSR_FUNC_( tlbrprmd )
_BUILD_LA_R_CSR_FUNC_( merrctl )
_BUILD_LA_R_CSR_FUNC_( merrinfo1 )
_BUILD_LA_R_CSR_FUNC_( merrinfo2 )
_BUILD_LA_R_CSR_FUNC_( merrentry )
_BUILD_LA_R_CSR_FUNC_( merrera )
_BUILD_LA_R_CSR_FUNC_( merrsave )
_BUILD_LA_R_CSR_FUNC_( ctag )
_BUILD_LA_R_CSR_FUNC_( msgis0 )
_BUILD_LA_R_CSR_FUNC_( msgis1 )
_BUILD_LA_R_CSR_FUNC_( msgis2 )
_BUILD_LA_R_CSR_FUNC_( msgis3 )
_BUILD_LA_R_CSR_FUNC_( msgir )
_BUILD_LA_R_CSR_FUNC_( msgie )
_BUILD_LA_R_CSR_FUNC_( dmwin0 )
_BUILD_LA_R_CSR_FUNC_( dmwin1 )
_BUILD_LA_R_CSR_FUNC_( dmwin2 )
_BUILD_LA_R_CSR_FUNC_( dmwin3 )


_LA_UINT64_( *_la_r_csr_[ _LA_CSR_MAX_NUM_ ] )( void ) = {
	[_la_crmd_] rFuncName( crmd ),
	[ _la_prmd_ ]		rFuncName( prmd ),
	[ _la_euen_ ]		rFuncName( euen ),
	[ _la_misc_ ]		rFuncName( misc ),
	[ _la_ecfg_ ]		rFuncName( ecfg ),
	[ _la_estat_ ]		rFuncName( estat ),
	[ _la_era_ ]		rFuncName( era ),
	[ _la_badv_ ]		rFuncName( badv ),
	[ _la_badi_ ]		rFuncName( badi ),
	[ _la_eentry_ ]		rFuncName( eentry ),
	[ _la_tlbidx_ ]		rFuncName( tlbidx ),
	[ _la_tlbehi_ ]		rFuncName( tlbehi ),
	[ _la_tlbelo0_ ]	rFuncName( tlbelo0 ),
	[ _la_tlbelo1_ ]	rFuncName( tlbelo1 ),
	[ _la_asid_ ]		rFuncName( asid ),
	[ _la_pgdl_ ]		rFuncName( pgdl ),
	[ _la_pgdh_ ] 		rFuncName( pgdh ), 
	[ _la_pgd_ ] 		rFuncName( pgd ),
	[ _la_pwcl_ ] 		rFuncName( pwcl ), 
	[ _la_pwch_ ] 		rFuncName( pwch ), 
	[ _la_stlbps_ ] 	rFuncName( stlbps ),
	[ _la_rvacfg_ ]		rFuncName( rvacfg ),
	[ _la_cpuid_ ] 		rFuncName( cpuid ), 
	[ _la_prcfg1_ ]		rFuncName( prcfg1 ),
	[ _la_prcfg2_ ]		rFuncName( prcfg2 ),
	[ _la_prcfg3_ ]		rFuncName( prcfg3 ),
	[ _la_save0_ ] 		rFuncName( save0 ), 
	[ _la_tid_ ]		rFuncName( tid ),
	[ _la_tcfg_ ]		rFuncName( tcfg ),
	[ _la_tval_ ]		rFuncName( tval ),
	[ _la_cntc_ ]		rFuncName( cntc ),
	[ _la_ticlr_ ]		rFuncName( ticlr ),
	[ _la_llbctl_ ]		rFuncName( llbctl ),
	[ _la_impctl1_ ]	rFuncName( impctl1 ),
	[ _la_impctl2_ ]	rFuncName( impctl2 ),
	[ _la_tlbrentry_ ] 	rFuncName( tlbrentry ), 
	[ _la_tlbrbadv_ ] 	rFuncName( tlbrbadv ), 
	[ _la_tlbrera_ ] 	rFuncName( tlbrera ), 
	[ _la_tlbrsave_ ] 	rFuncName( tlbrsave ), 
	[ _la_tlbrelo0_ ] 	rFuncName( tlbrelo0 ), 
	[ _la_tlbrelo1_ ] 	rFuncName( tlbrelo1 ), 
	[ _la_tlbrehi_ ] 	rFuncName( tlbrehi ),
	[ _la_tlbrprmd_ ] 	rFuncName( tlbrprmd ),
	[ _la_merrctl_ ] 	rFuncName( merrctl ),
	[ _la_merrinfo1_ ] 	rFuncName( merrinfo1 ),
	[ _la_merrinfo2_ ] 	rFuncName( merrinfo2 ),
	[ _la_merrentry_ ] 	rFuncName( merrentry ),
	[ _la_merrera_ ] 	rFuncName( merrera ),
	[ _la_merrsave_ ] 	rFuncName( merrsave ),
	[ _la_ctag_ ] 		rFuncName( ctag ),
	[ _la_msgis0_ ] 	rFuncName( msgis0 ),
	[ _la_msgis1_ ] 	rFuncName( msgis1 ),
	[ _la_msgis2_ ] 	rFuncName( msgis2 ),
	[ _la_msgis3_ ] 	rFuncName( msgis3 ),
	[ _la_msgir_ ] 		rFuncName( msgir ),
	[ _la_msgie_ ] 		rFuncName( msgie ),
	[ _la_dmwin0_ ] 	rFuncName( dmwin0 ),
	[ _la_dmwin1_ ] 	rFuncName( dmwin1 ),
	[ _la_dmwin2_ ] 	rFuncName( dmwin2 ),
	[ _la_dmwin3_ ] 	rFuncName( dmwin3 ),
};