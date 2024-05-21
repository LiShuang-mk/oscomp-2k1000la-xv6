//
// Created by Li shuang ( pseudonym ) on 2024-03-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hal/loongarch.hh"
#include "hal/cpu.hh"
#include "klib/klib.hh"
#include "mm/virtual_memory_manager.hh"
#include "mm/physical_memory_manager.hh"
#include "mm/page.hh"
#include "mm/page_table.hh"
#include "mm/memlayout.hh"
#include "mm/tlb_manager.hh"
#include "pm/process.hh"
#include "klib/klib.hh"
namespace mm
{
	VirtualMemoryManager k_vmm;

	uint64 VirtualMemoryManager::kstack_vm_from_gid( uint gid )
	{
		if ( gid >= pm::num_process )
			log_panic( "vmm: invalid gid" );
		return ( vml::vm_trap_frame - ( ( ( gid + 1 ) << 1 ) << pg_size_shift ) );
	}

	void VirtualMemoryManager::init( const char *lock_name )
	{
		_lock.init( lock_name );

		k_pagetable.set_global();

		uint64 addr = ( uint64 ) k_pmm.alloc_page();
		if ( addr == 0 )
			log_panic( "vmm init" );
		k_pmm.clear_page( ( void* ) addr );
		k_pagetable.set_base( addr );

		loongarch::Cpu::write_csr( loongarch::csr::pgdl, addr );
		loongarch::Cpu::write_csr( loongarch::csr::pgdh, addr );
		k_tlbm.init( "tlb manager" );

		log_info( "pgdl : %p", loongarch::Cpu::read_csr( loongarch::csr::pgdl ) );
		log_info( "pgdh : %p", loongarch::Cpu::read_csr( loongarch::csr::pgdh ) );

		for ( pm::Pcb &pcb : pm::k_proc_pool )
		{
			pcb.map_kstack( mm::k_pagetable );
		}


		loongarch::Cpu::write_csr(
			loongarch::csr::pwcl,
			( pte_width << 30 ) |
			( pt_bits_width << 25 ) |
			( dir2_vpn_shift << 20 ) |
			( pt_bits_width << 15 ) |
			( dir1_vpn_shift << 10 ) |
			( pt_bits_width << 5 ) |
			( pt_vpn_shift << 0 )
		);

		loongarch::Cpu::write_csr(
			loongarch::csr::pwch,
			( pt_bits_null << 18 ) |
			( dir4_vpn_shift << 12 ) |
			( pt_bits_width << 6 ) |
			( dir3_vpn_shift << 0 )
		);
	}

	bool VirtualMemoryManager::map_pages( PageTable &pt, uint64 va, uint64 size, uint64 pa, flag_t flags )
	{
		uint64 a, last;
		Pte pte;

		if ( size == 0 )
			log_panic( "mappages: size" );

		a = page_round_down( va );
		last = page_round_down( va + size - 1 );
		for ( ;;)
		{
			pte = pt.walk( a, /*alloc*/ true );
			if ( pte.is_null() )
			{
				log_warn( "walk failed" );
				return false;
			}
			if ( pte.is_valid() )
				log_panic( "mappages: remap" );
			// pte = PA2PTE( pa ) | perm | loongarch::PteEnum::valid_m;
			pte.set_data( page_round_down( pa ) | flags | loongarch::PteEnum::valid_m );
			if ( a == last )
				break;
			a += pg_size;
			pa += pg_size;
		}
		return true;
	}

	uint64 VirtualMemoryManager::vmalloc( PageTable &pt, uint64 old_sz, uint64 new_sz )
	{
		void *mem;

		if ( new_sz < old_sz )
			return old_sz;

		old_sz = page_round_up( old_sz );
		for ( uint64 a = old_sz; a < new_sz; a += pg_size )
		{
			mem = k_pmm.alloc_page();
			if ( mem == nullptr )
			{
				vmdealloc( pt, a, old_sz );
				return 0;
			}
			k_pmm.clear_page( mem );
			if ( map_pages( pt, a, pg_size, ( uint64 ) mem,
				loongarch::PteEnum::presence_m |
				loongarch::PteEnum::writable_m |
				loongarch::PteEnum::dirty_m |
				( loongarch::mat_cc << loongarch::PteEnum::mat_s ) |
				( 0x3 << loongarch::PteEnum::plv_s )
			) == false )
			{
				k_pmm.free_page( mem );
				vmdealloc( pt, a, old_sz );
				return 0;
			}
		}
		return new_sz;
	}

	uint64 VirtualMemoryManager::vmdealloc( PageTable &pt, uint64 old_sz, uint64 new_sz )
	{
		if ( new_sz >= old_sz )
			return old_sz;

		if ( page_round_up( new_sz ) < page_round_up( old_sz ) )
		{
			int npages = ( page_round_up( old_sz ) - page_round_up( new_sz ) ) / pg_size;
			vmunmap( pt, page_round_up( new_sz ), npages, true );
		}

		return new_sz;
	}

	int VirtualMemoryManager::copy_in( PageTable &pt, void *dst, uint64 src_va, uint64 len )
	{
		uint64 n, va, pa;
		char *p_dst = ( char * ) dst;

		while ( len > 0 )
		{
			va = page_round_down( src_va );
			pa = ( uint64 ) pt.walk_addr( va );
			if ( pa == 0 )
				return -1;
			n = pg_size - ( src_va - va );
			if ( n > len )
				n = len;
			memmove( ( void * ) p_dst, ( const void * ) ( pa + ( src_va - va ) ), n );

			len -= n;
			p_dst += n;
			src_va = va + pg_size;
		}
		return 0;
	}

	int VirtualMemoryManager::copy_str_in( PageTable &pt, void *dst, uint64 src_va, uint64 max )
	{
		uint64 n, va, pa;
		int got_null = 0;
		char *p_dst = ( char * ) dst;

		while ( got_null == 0 && max > 0 )
		{
			va = page_round_down( src_va );
			pa = ( uint64 ) pt.walk_addr( va );
			if ( pa == 0 )
				return -1;
			n = pg_size - ( src_va - va );
			if ( n > max )
				n = max;

			char *p = ( char * ) ( pa + ( src_va - va ) );
			while ( n > 0 )
			{
				if ( *p == '\0' )
				{
					*p_dst = '\0';
					got_null = 1;
					break;
				}
				else
				{
					*p_dst = *p;
				}
				--n;
				--max;
				p++;
				p_dst++;
			}

			src_va = va + pg_size;
		}
		if ( got_null )
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}

	int VirtualMemoryManager::either_copy_in( void *dst, bool user_src, uint64 src, uint64 len )
	{
		if ( user_src )
		{
			pm::Pcb *p = loongarch::Cpu::get_cpu()->get_cur_proc();
			mm::PageTable pt = p->get_pagetable();
			return copy_in( pt, dst, src, len );
		}
		else
		{
			memmove( dst, ( const char * ) src, len );
			return 0;
		}
	}

	uint64 VirtualMemoryManager::allocshm( PageTable &pt, uint64 oldshm, uint64 newshm, uint64 sz, void *phyaddr[ pm::MAX_SHM_PGNUM ] )
	{
		void *mem;
		uint64 a;

		if ( oldshm & 0xfff || newshm & 0xfff || newshm < sz || oldshm >( vm_trap_frame - 64 * 2 * pg_size ) )
		{
			log_panic( "allocshm: bad parameters" );
			return 0;
		}
		a = newshm;
		for ( int i = 0; a < oldshm; a += pg_size, i++ )
		{
			mem = k_pmm.alloc_page();
			if ( mem == nullptr )
			{
				log_panic( "allocshm: no memory" );
				deallocshm( pt, newshm, a );
				return 0;
			}
			map_pages( pt, a, pg_size, uint64( phyaddr[ i ] ), loongarch::PteEnum::presence_m |
				loongarch::PteEnum::writable_m | loongarch::PteEnum::plv_m | loongarch::PteEnum::mat_m | loongarch::PteEnum::dirty_m );
			phyaddr[ i ] = mem;
			printf( "allocshm: %p => %p\n", a, phyaddr[ i ] );
		}
		return newshm;
	}

	uint64 VirtualMemoryManager::mapshm( PageTable &pt, uint64 oldshm, uint64 newshm, uint sz, void **phyaddr )
	{
		uint64 a;
		if ( oldshm & 0xfff || newshm & 0xfff || newshm < sz || oldshm >( vm_trap_frame - 64 * 2 * pg_size ) )
		{
			log_panic( "mapshm: bad parameters when shmmap" );
			return 0;
		}
		a = newshm;
		for ( int i = 0; a < oldshm; a += pg_size, i++ )
		{
			map_pages( pt, a, pg_size, uint64( phyaddr[ i ] ), loongarch::PteEnum::presence_m |
				loongarch::PteEnum::writable_m | loongarch::PteEnum::plv_m | loongarch::PteEnum::mat_m | loongarch::PteEnum::dirty_m );
			printf( "mapshm: %p => %p\n", a, phyaddr[ i ] );
		}
		return newshm;
	}

	uint64 VirtualMemoryManager::deallocshm( PageTable &pt, uint64 oldshm, uint64 newshm )
	{
		if ( newshm <= oldshm )
			return oldshm;

		if ( page_round_up( newshm ) > page_round_up( oldshm ) )
		{
			int npages = page_round_up( newshm ) - page_round_up( oldshm ) / pg_size;
			vmunmap( pt, page_round_up( oldshm ), npages, 0 );
		}
		return oldshm;
	}

	int VirtualMemoryManager::copyout( PageTable &pt, uint64 va, const void *p, uint64 len )
	{
		uint64 n, a, pa;

		while ( len > 0 )
		{
			a = page_round_down( va );
			Pte pte = pt.walk( a, 0 );
			pa = reinterpret_cast< uint64 >( pte.pa() );
			if ( pa == 0 )
				return -1;
			n = pg_size - ( va - a );
			if ( n > len )
				n = len;
			memmove( ( void * ) ( ( pa + ( va - a ) ) | loongarch::qemuls2k::dmwin::win_0 ), p, n );

			len -= n;
			p = ( char* ) p + n;
			va = a + pg_size;
		}
		return 0;
	}

	void VirtualMemoryManager::vmunmap( PageTable &pt, uint64 va, uint64 npages, int do_free )
	{
		uint64 a;
		Pte pte;

		if ( ( va % PageEnum::pg_size ) != 0 )
			log_panic( "vmunmap: not aligned" );

		for ( a = va; a < va + npages * PageEnum::pg_size; a += PageEnum::pg_size )
		{
			if ( ( pte = pt.walk( a, 0 ) ).is_null() )
				log_panic( "vmunmap: walk" );
			if ( !pte.is_valid() )
				log_panic( "vmunmap: not mapped" );
			if ( !pte.is_leaf() )
				log_panic( "vmunmap: not a leaf" );
			if ( do_free )
			{
				k_pmm.free_page( pte.pa() );
			}
			pte = 0;
		}
	}

	PageTable VirtualMemoryManager::vm_create()
	{
		PageTable pt;
		pt.set_global();

		uint64 addr = ( uint64 ) k_pmm.alloc_page();
		if ( addr == 0 )
			log_panic( "vmm: no mem to crate vm space." );
		k_pmm.clear_page( ( void* ) addr );
		pt.set_base( addr );

		return pt;
	}

	int VirtualMemoryManager::vm_copy( PageTable &old_pt, PageTable &new_pt, uint64 size )
	{
		Pte pte;
		uint64 pa, va;
		uint64 flags;
		void *mem;

		for ( va = 0; va < size; va += pg_size )
		{
			if ( ( pte = old_pt.walk( va, 0 ) ).is_null() )
				log_panic( "uvmcopy: pte should exist" );
			if ( !pte.is_present() )
				log_panic( "uvmcopy: page not present" );
			pa = ( uint64 ) pte.pa();
			flags = pte.flags();
			if ( ( mem = mm::k_pmm.alloc_page() ) == nullptr )
			{
				vmunmap( new_pt, 0, va / pg_size, 1 );
				return -1;
			}
			memmove( mem, ( const char * ) pa, pg_size );
			if ( map_pages( new_pt, va, pg_size, ( uint64 ) mem, flags ) == false )
			{
				mm::k_pmm.free_page( mem );
				vmunmap( new_pt, 0, va / pg_size, 1 );
				return -1;
			}
		}
		return 0;
	}

	void VirtualMemoryManager::vmfree( PageTable &pt, uint64 sz )
	{
		if ( sz > 0 )
			vmunmap( pt, 0, page_round_up( sz ) / PageEnum::pg_size, 1 );
		pt.freewalk();
	}

	void VirtualMemoryManager::uvmclear( PageTable &pt, uint64 va )
	{
		Pte pte = pt.walk( va, 0 );
		if ( pte.is_valid() )
			pte.unset_plv();
	}

	uint64 VirtualMemoryManager::uvmalloc( PageTable &pt, uint64 oldsz, uint64 newsz )
	{
		uint64 a;
		uint64 pa;

		if ( newsz < oldsz )  // shrink, not here
			return oldsz;
		a = page_round_up( oldsz ); // start from the next page

		for ( a = oldsz; a < newsz; a += pg_size )
		{
			pa = ( uint64 ) mm::k_pmm.alloc_page();
			if ( pa == 0 )
			{
				vmfree( pt, oldsz );
				return 0;
			}
			if ( !map_pages( pt, a, pg_size, pa,
				( loongarch::PteEnum::presence_m ) |
				( loongarch::PteEnum::writable_m ) |
				( loongarch::PteEnum::plv_m ) |
				( loongarch::PteEnum::mat_m ) |
				( loongarch::PteEnum::dirty_m ) ) )
			{
				k_pmm.free_page( ( void * ) pa );
				uvmdealloc( pt, a, oldsz );
				return 0;
			}
		}
		return newsz;
	}

	uint64 VirtualMemoryManager::uvmdealloc( PageTable &pt, uint64 oldsz, uint64 newsz )
	{
		if ( newsz >= oldsz )
			return oldsz;
		if ( page_round_up( newsz ) < page_round_up( oldsz ) )
			vmunmap( pt,
				page_round_up( newsz ),
				( page_round_up( oldsz ) - page_round_up( newsz ) ) / mm::pg_size,
				1 );
		return newsz;
	}


}