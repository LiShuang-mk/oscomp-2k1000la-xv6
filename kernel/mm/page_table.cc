//
// Created by Li shuang ( pseudonym ) on 2024-04-02 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "mm/physical_memory_manager.hh"
#include "mm/page_table.hh"
#include "mm/pte.hh"
#include "mm/memlayout.hh"
#include "mm/page.hh"
#include "klib/common.hh"

namespace mm
{
	PageTable k_pagetable;

	bool debug_trace_walk = false;

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

		if ( debug_trace_walk )
			printf( "[walk trace] 0x%x : ", va );
		uint64 pg_num;

		// search in level-3 
		pg_num = pt.dir3_num( va );
		pte = pt.get_pte( pg_num );
		if ( debug_trace_walk )
			printf( "0x%x->", pte.get_data() );
		if ( !_walk_to_next_level( pte, alloc, pt ) )
		{
			log_warn( "walk fail" );
			return Pte();
		}
		// search in level-2 
		pg_num = pt.dir2_num( va );
		pte = pt.get_pte( pg_num );
		if ( debug_trace_walk )
			printf( "0x%x->", pte.get_data() );
		if ( !_walk_to_next_level( pte, alloc, pt ) )
		{
			log_warn( "walk fail" );
			return Pte();
		}
		// search in level-1 
		pg_num = pt.dir1_num( va );
		pte = pt.get_pte( pg_num );
		if ( debug_trace_walk )
			printf( "0x%x->", pte.get_data() );
		if ( !_walk_to_next_level( pte, alloc, pt ) )
		{
			log_warn( "walk fail" );
			return Pte();
		}

		pg_num = pt.pt_num( va );
		pte = pt.get_pte( pg_num );
		if ( debug_trace_walk )
			printf( "0x%x\n", pte.get_data() );
		return pte;
	}

	void * PageTable::walk_addr( uint64 va )
	{
		uint64 pa;

		if ( va >= vml::vm_end )
			return 0;

		Pte pte = walk( va, false/* alloc */ );
		if ( pte._data_addr == nullptr )
			return nullptr;
		if ( !pte.is_valid() )
			return nullptr;
		if ( pte.plv() == 0 )
		{
			log_warn( "try to walk-addr( k-pt, %p ). nullptr will be return.", va );
			return nullptr;
		}
		pa = ( uint64 ) pte.pa();
		return ( void * ) pa;
	}


	void PageTable::freewalk()
	{ // pte num is 4096 / 8 = 512 in pgtable
		for ( uint i = 0; i < 512; i++ )
		{
			Pte next_level = get_pte( i );
			Pte _pte( ( pte_t * ) next_level.pa() );
			bool pte_valid = _pte.is_valid();
			bool pte_leaf = _pte.is_leaf();
			// if ( _pte.is_valid() && !_pte.is_leaf() )      // PGT -> PTE -> _pte
			if ( pte_valid && !pte_leaf )
			{																							//  get_pte_addr
				// this PTE is points to a lower-level page table
				PageTable child;
				child.set_base( ( uint64 ) _pte.pa() );
				child.freewalk();
				reset_pte_data( i );
			}
			else if ( pte_valid )
			{
				log_panic( "freewalk: leaf" );
			}
		}
		k_pmm.free_page( ( void * ) _base_addr );
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


// ---------------- private helper functions ----------------

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
			pte.set_data( page_round_down(
				loongarch::qemuls2k::virt_to_phy_address( ( uint64 ) page_addr ) )
				| loongarch::PteEnum::valid_m );
		}
		return true;
	}
}