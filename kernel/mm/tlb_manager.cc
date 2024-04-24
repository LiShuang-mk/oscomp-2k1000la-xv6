//
// Created by Li shuang ( pseudonym ) on 2024-04-04 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "mm/tlb_manager.hh"
#include "mm/page.hh"
#include "hal/cpu.hh"

namespace mm
{
	TlbManager k_tlbm;

	void mm::TlbManager::init( const char * lock_name )
	{
		_lock.init( lock_name );
		invalid_all_tlb();
		loongarch::Cpu::write_csr( loongarch::csr::CsrAddr::stlbps, PageEnum::pg_size_shift );
		loongarch::Cpu::write_csr( loongarch::csr::CsrAddr::asid, 0x0UL );
		loongarch::Cpu::write_csr( loongarch::csr::CsrAddr::tlbrehi, PageEnum::pg_size_shift );
	}
} // namespace mm

