//
// Created by Li shuang ( pseudonym ) on 2024-04-02 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "mm/physical_memory_manager.hh"
#include "mm/page_table.hh"
#include "mm/memlayout.hh"
#include "mm/page.hh"
#include "klib/common.hh"

namespace mm
{
	PageTable k_pagetable;

	PageTable::PageTable()
		: _base_addr( 0 )
	{
		
	}

	Pte PageTable::walk( uint64 va, bool alloc )
	{
		if ( !_is_global )
		{
			log_warn( "walk: pagetable not global" );
			return Pte();
		}
		if ( va >= vml::vm_end )
			log_panic( "va out of bounds" );

		PageTable pt;
		pt.set_base( _base_addr );
		Pte pte;

		// search in level-3 
		pte = pt.get_pte( pt.dir3_num( va ) );
		if ( !_walk_to_next_level( pte, alloc, pt ) )
		{
			log_warn( "walk fail" );
			return Pte();
		}
		// search in level-2 
		pte = pt.get_pte( pt.dir2_num( va ) );
		if ( !_walk_to_next_level( pte, alloc, pt ) )
		{
			log_warn( "walk fail" );
			return Pte();
		}
		// search in level-1 
		pte = pt.get_pte( pt.dir1_num( va ) );
		if ( !_walk_to_next_level( pte, alloc, pt ) )
		{
			log_warn( "walk fail" );
			return Pte();
		}

		pte = pt.get_pte( pt.pt_num( va ) );
		return pte;
	}

	bool PageTable::_walk_to_next_level( Pte pte, bool alloc, PageTable &pt )
	{
		if ( pte.is_valid() )
		{
			pt.set_base( ( uint64 ) pte.pa() );
		}
		else
		{
			if ( !alloc )
			{
				log_panic( "try to walk to next level but next level page table is not alloced." );
				return false;
			}
			void *page_addr = k_pmm.alloc_page();
			if ( page_addr == 0 )
			{
				log_warn( "physical page alloc failed." );
				return false;
			}
			k_pmm.clear_page( page_addr );
			pt.set_base( ( uint64 ) page_addr );
			pte.set_data( page_round_down( ( uint64 ) page_addr ) | loongarch::PteEnum::valid_m );
		}
		return true;
	}

	void PageTable::freewalk()
	{ // pte num is 4096 / 8 = 512 in pgtable
		for(uint i=0; i< 512;i++)
		{
			uint64 pte_data = get_pte_data(i);
			Pte _pte((pte_t *)pte_data);
			if(_pte.is_valid() && ! _pte.is_leaf())      // PGT -> PTE -> _pte
			{																							//  get_pte_addr
				// this PTE is points to a lower-level page table
				PageTable child;
				child.set_base( ( uint64 ) _pte.pa());
				child.freewalk();
				reset_pte_data(i);
			}
			else if( _pte.is_valid() )
			{
				log_panic("freewalk: leaf");
			}
		}
		k_pmm.free_page((void *)_base_addr);
	}

	uint64 PageTable::dir3_num( uint64 va )
	{
		return ( va & PageEnum::dir3_vpn_mask ) >> PageEnum::dir3_vpn_shift;
	}
	uint64 PageTable::dir2_num( uint64 va )
	{
		return ( va & PageEnum::dir2_vpn_mask ) >> PageEnum::dir2_vpn_shift;
	}
	uint64 PageTable::dir1_num( uint64 va )
	{
		return ( va & PageEnum::dir1_vpn_mask ) >> PageEnum::dir1_vpn_shift;
	}
	uint64 PageTable::pt_num( uint64 va )
	{
		return ( va & PageEnum::pt_vpn_mask ) >> PageEnum::pt_vpn_shift;
	}
}