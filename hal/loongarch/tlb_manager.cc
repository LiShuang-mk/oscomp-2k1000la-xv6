//
// Created by Li shuang ( pseudonym ) on 2024-04-04 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "tlb_manager.hh"
#include "loongarch.hh"
#include "la_cpu.hh"

#include <hsai_global.hh>
#include <virtual_cpu.hh>
#include <mem/page.hh>

namespace loongarch
{
	TlbManager k_tlbm;

	void TlbManager::init( const char * lock_name )
	{
		_lock.init( lock_name );
		invalid_all_tlb();
		Cpu * cpu = ( Cpu * ) hsai::get_cpu();
		cpu->write_csr( csr::stlbps, hsai::page_size_shift );
		cpu->write_csr( csr::asid, 0x0UL );
		cpu->write_csr( csr::tlbrehi, hsai::page_size_shift );
	}
} // namespace mm

