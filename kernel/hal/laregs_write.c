//
// Created by Li shuang ( pseudonym ) on 2024-04-04 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hal/laregs.h"

_BUILD_LA_W_CSR_FUNC_( crmd )
_BUILD_LA_W_CSR_FUNC_( tlbehi )
_BUILD_LA_W_CSR_FUNC_( asid )
_BUILD_LA_W_CSR_FUNC_( pgdl )
_BUILD_LA_W_CSR_FUNC_( pgdh )
_BUILD_LA_W_CSR_FUNC_( pgd )
_BUILD_LA_W_CSR_FUNC_( pwcl )
_BUILD_LA_W_CSR_FUNC_( pwch )
_BUILD_LA_W_CSR_FUNC_( stlbps )
_BUILD_LA_W_CSR_FUNC_( cpuid )
_BUILD_LA_W_CSR_FUNC_( save0 )
_BUILD_LA_W_CSR_FUNC_( tlbrentry )
_BUILD_LA_W_CSR_FUNC_( tlbrbadv )
_BUILD_LA_W_CSR_FUNC_( tlbrsave )
_BUILD_LA_W_CSR_FUNC_( tlbrehi )
_BUILD_LA_W_CSR_FUNC_( dmwin0 )
_BUILD_LA_W_CSR_FUNC_( dmwin1 )
_BUILD_LA_W_CSR_FUNC_( dmwin2 )
_BUILD_LA_W_CSR_FUNC_( dmwin3 )

void( *_la_w_csr_[ _LA_CSR_MAX_NUM_ ] )( _LA_UINT64_ ) = {
	[_la_crmd_] wFuncName( crmd ),
	[ _la_tlbehi_ ]		wFuncName( tlbehi ),
	[ _la_asid_ ]		wFuncName( asid ),
	[ _la_pgdl_ ]		wFuncName( pgdl ),
	[ _la_pgdh_ ] 		wFuncName( pgdh ), 
	[ _la_pgd_ ] 		wFuncName( pgd ), 
	[ _la_pwcl_ ] 		wFuncName( pwcl ), 
	[ _la_pwch_ ] 		wFuncName( pwch ), 
	[ _la_stlbps_ ] 	wFuncName( stlbps ), 
	[ _la_cpuid_ ] 		wFuncName( cpuid ), 
	[ _la_save0_ ] 		wFuncName( save0 ), 
	[ _la_tlbrentry_ ] 	wFuncName( tlbrentry ), 
	[ _la_tlbrbadv_ ] 	wFuncName( tlbrbadv ), 
	[ _la_tlbrsave_ ] 	wFuncName( tlbrsave ), 
	[ _la_tlbrehi_ ] 	wFuncName( tlbrehi ),
	[ _la_dmwin0_ ] 	wFuncName( dmwin0 ),
	[ _la_dmwin1_ ] 	wFuncName( dmwin1 ),
	[ _la_dmwin2_ ] 	wFuncName( dmwin2 ),
	[ _la_dmwin3_ ] 	wFuncName( dmwin3 ),
};