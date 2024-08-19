//
// Created by Li shuang ( pseudonym ) on 2024-04-02
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#include "mm/page_table.hh"

#include <hsai_global.hh>
#include <mem/virtual_memory.hh>

#include "klib/common.hh"
#include "mm/memlayout.hh"
#include "mm/page.hh"
#include "mm/physical_memory_manager.hh"

#if PT_LEVEL != 4
#	error "只支持4级页表遍历"
#endif
namespace mm
{
	PageTable k_pagetable;

	bool debug_trace_walk = false;

	hsai::Pte PageTable::walk( uint64 va, bool alloc )
	{
		if ( !_is_global )
		{
			log_warn( "walk: pagetable not global" );
			return hsai::Pte();
		}
		// if ( va >= vml::vm_end )
		// log_panic( "va out of bounds" );

		PageTable pt;
		pt.set_base( _base_addr );
		hsai::Pte pte;

		if ( debug_trace_walk ) printf( "[walk trace] 0x%x : ", va );
		uint64 pg_num;

		// search in level-3
		pg_num = hsai::pgd_num( va );
		pte	   = pt.get_pte( pg_num );
		if ( debug_trace_walk ) printf( "0x%x->", pte.get_data() );
		if ( !_walk_to_next_level( pte, alloc, pt ) )
		{
			log_panic( "walk pgd to pud fail, va=%p, pgd-base=%p, pgd-base=%p", va, _base_addr,
					   pt._base_addr );
			return hsai::Pte();
		}
		// search in level-2
		pg_num = hsai::pud_num( va );
		pte	   = pt.get_pte( pg_num );
		if ( debug_trace_walk ) printf( "0x%x->", pte.get_data() );
		if ( !_walk_to_next_level( pte, alloc, pt ) )
		{
			log_panic( "walk pud to pmd fail, va=%p, pgd-base=%p, pud-base=%p", va, _base_addr,
					   pt._base_addr );
			return hsai::Pte();
		}
		// search in level-1
		pg_num = hsai::pmd_num( va );
		pte	   = pt.get_pte( pg_num );
		if ( debug_trace_walk ) printf( "0x%x->", pte.get_data() );
		if ( !_walk_to_next_level( pte, alloc, pt ) )
		{
			log_panic( "walk pmd to pt fail, va=%p, pgd-base=%p, pmd-base=%p", va, _base_addr,
					   pt._base_addr );
			return hsai::Pte();
		}

		pg_num = hsai::pt_num( va );
		pte	   = pt.get_pte( pg_num );
		if ( debug_trace_walk ) printf( "0x%x\n", pte.get_data() );
		return pte;
	}

	ulong PageTable::walk_addr( uint64 va )
	{
		uint64 pa;

		// if ( va >= vml::vm_end )
		// return 0;

		hsai::Pte pte = walk( va, false /* alloc */ );
		if ( pte.is_null() ) return 0;
		if ( !pte.is_valid() ) return 0;
		if ( pte.is_super_plv() )
		{
			log_warn( "try to walk-addr( k-pt, %p ). nullptr will be return.", va );
			return 0;
		}
		pa	= (uint64) pte.to_pa();
		pa |= va & ( hsai::page_size - 1 );
		return pa;
	}


	void PageTable::freewalk()
	{
		uint pte_cnt = hsai::page_size / sizeof( pte_t );
		for ( uint i = 0; i < pte_cnt; i++ )
		{
			hsai::Pte next_level = get_pte( i );
			bool	  pte_valid = next_level.is_valid();
			bool	  pte_leaf	= !next_level.is_dir_page();
			// if ( _pte.is_valid() && !_pte.is_leaf() )      // PGT -> PTE -> _pte
			if ( pte_valid && !pte_leaf )
			{ //  get_pte_addr
				// this PTE is points to a lower-level page table
				PageTable child;
				child.set_base( hsai::k_mem->to_vir( next_level.to_pa() ) );
				child.freewalk();
				reset_pte_data( i );
			}
			else if ( pte_valid )
			{
				log_trace( "freewalk trace pt data:" );
				hsai_printf( BLUE_COLOR_PRINT
							 "pt\t----++++----+++0 ----++++----+++1 ----++++----+++2 "
							 "----++++----+++3\n" CLEAR_COLOR_PRINT );
				u64 *buf = (u64 *) _base_addr;
				for ( uint i = 0; i < hsai::page_size / sizeof( u64 ); ++i )
				{
					if ( i % 4 == 0 ) hsai_printf( "\t", i >> 8, i );
					hsai_printf( "%016lx ", buf[i] );
					if ( i % 4 == 3 ) hsai_printf( "\n" );
				}
				log_panic( "freewalk: leaf. pte no. = %d", i );
			}
		}
		k_pmm.free_pages( (void *) _base_addr );
	}

	void PageTable::freewalk_mapped()
	{
		uint pte_cnt = hsai::page_size / sizeof( pte_t );
		for ( uint i = 0; i < pte_cnt; i++ )
		{
			hsai::Pte next_level = get_pte( i );
			hsai::Pte _pte( (pte_t *) next_level.to_pa() );
			bool	  pte_dir = _pte.is_dir_page();
			if ( pte_dir ) // 如果pte指向一个页表，则递归释放
			{
				// this PTE is points to a lower-level page table
				PageTable child;
				child.set_base( hsai::k_mem->to_vir( _pte.to_pa() ) );
				child.freewalk();
				reset_pte_data( i );
			}
			else if ( _pte.is_valid() ) // 如果pte指向一个有效的物理页面，则将页面释放
			{
				k_pmm.free_pages( (void *) hsai::k_mem->to_vir( _pte.to_pa() ) );
			}
		}
		k_pmm.free_pages( (void *) _base_addr );
	}


	// ---------------- private helper functions ----------------

	bool PageTable::_walk_to_next_level( hsai::Pte pte, bool alloc, PageTable &pt )
	{
		if ( pte.is_valid() ) { pt.set_base( hsai::k_mem->to_vir( pte.to_pa() ) ); }
		else
		{
			if ( !alloc )
			{
				log_error( "try to walk to next level but next level page table is not alloced." );
				return false;
			}
			void *page_addr = k_pmm.alloc_page();
			if ( page_addr == 0 )
			{
				log_warn( "physical page alloc failed." );
				return false;
			}
			k_pmm.clear_page( page_addr );
			pt.set_base( (uint64) page_addr );
			pte.set_data( hsai::page_round_down( hsai::k_mem->to_phy( (ulong) page_addr ) ) |
						  hsai::Pte::map_dir_page_flags() );
		}
		return true;
	}
} // namespace mm
