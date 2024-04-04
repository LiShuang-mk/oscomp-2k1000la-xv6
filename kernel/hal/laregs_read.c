//
// Created by Li shuang ( pseudonym ) on 2024-04-04 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hal/laregs.h"

_BUILD_LA_R_CSR_FUNC_( crmd )
_BUILD_LA_R_CSR_FUNC_( tlbehi )
_BUILD_LA_R_CSR_FUNC_( asid )
_BUILD_LA_R_CSR_FUNC_( pgdl )
_BUILD_LA_R_CSR_FUNC_( pgdh )
_BUILD_LA_R_CSR_FUNC_( pgd )
_BUILD_LA_R_CSR_FUNC_( pwcl )
_BUILD_LA_R_CSR_FUNC_( pwch )
_BUILD_LA_R_CSR_FUNC_( stlbps )
_BUILD_LA_R_CSR_FUNC_( cpuid )
_BUILD_LA_R_CSR_FUNC_( save0 )
_BUILD_LA_R_CSR_FUNC_( tlbrentry )
_BUILD_LA_R_CSR_FUNC_( tlbrbadv )
_BUILD_LA_R_CSR_FUNC_( tlbrsave )
_BUILD_LA_R_CSR_FUNC_( tlbrehi )
_BUILD_LA_R_CSR_FUNC_( dmwin0 )
_BUILD_LA_R_CSR_FUNC_( dmwin1 )
_BUILD_LA_R_CSR_FUNC_( dmwin2 )
_BUILD_LA_R_CSR_FUNC_( dmwin3 )


_LA_UINT64_( *_la_r_csr_[ _LA_CSR_MAX_NUM_ ] )( void ) = {
	[_la_crmd_] rFuncName( crmd ),
	[ _la_tlbehi_ ]		rFuncName( tlbehi ),
	[ _la_asid_ ]		rFuncName( asid ),
	[ _la_pgdl_ ]		rFuncName( pgdl ),
	[ _la_pgdh_ ] 		rFuncName( pgdh ), 
	[ _la_pgd_ ] 		rFuncName( pgd ),
	[ _la_pwcl_ ] 		rFuncName( pwcl ), 
	[ _la_pwch_ ] 		rFuncName( pwch ), 
	[ _la_stlbps_ ] 	rFuncName( stlbps ),
	[ _la_cpuid_ ] 		rFuncName( cpuid ), 
	[ _la_save0_ ] 		rFuncName( save0 ), 
	[ _la_tlbrentry_ ] 	rFuncName( tlbrentry ), 
	[ _la_tlbrbadv_ ] 	rFuncName( tlbrbadv ), 
	[ _la_tlbrsave_ ] 	rFuncName( tlbrsave ), 
	[ _la_tlbrehi_ ] 	rFuncName( tlbrehi ),
	[ _la_dmwin0_ ] 	rFuncName( dmwin0 ),
	[ _la_dmwin1_ ] 	rFuncName( dmwin1 ),
	[ _la_dmwin2_ ] 	rFuncName( dmwin2 ),
	[ _la_dmwin3_ ] 	rFuncName( dmwin3 ),
};
