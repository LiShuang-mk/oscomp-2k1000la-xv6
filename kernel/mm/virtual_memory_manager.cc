//
// Created by Li shuang ( pseudonym ) on 2024-03-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "klib/klib.hh"
#include "mm/virtual_memory_manager.hh"
#include "mm/physical_memory_manager.hh"
#include "mm/page.hh"
#include "mm/page_table.hh"
#include "mm/memlayout.hh"
#include "pm/process.hh"
#include "klib/klib.hh"

#include <hsai_global.hh>
#include <mem/virtual_memory.hh>
#include <process_interface.hh>
namespace mm
{
	VirtualMemoryManager k_vmm;

	uint64 VirtualMemoryManager::kstack_vm_from_gid( uint gid )
	{
		if ( gid >= pm::num_process )
			log_panic( "vmm: invalid gid" );
		return ( vml::vm_trap_frame - ( ( ( gid + 1 ) * ( pm::default_proc_kstack_pages + 1 ) ) << hsai::page_size_shift ) );
	}

	void VirtualMemoryManager::init( const char *lock_name )
	{
		_lock.init( lock_name );

		new ( &k_pagetable ) PageTable;

		k_pagetable.set_global();

		uint64 addr = ( uint64 ) k_pmm.alloc_page();
		if ( addr == 0 )
			log_panic( "vmm init" );
		k_pmm.clear_page( ( void* ) addr );
		k_pagetable.set_base( addr );

		hsai::k_mem->config_pt( addr );

		for ( pm::Pcb &pcb : pm::k_proc_pool )
		{
			pcb.map_kstack( mm::k_pagetable );
		}
	}

	bool VirtualMemoryManager::map_pages( PageTable &pt, uint64 va, uint64 size, uint64 pa, flag_t flags )
	{
		uint64 a, last;
		hsai::Pte pte;

		if ( size == 0 )
			log_panic( "mappages: size" );

		a = hsai::page_round_down( va );
		last = hsai::page_round_down( va + size - 1 );
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

			pte.set_data( hsai::page_round_down( hsai::k_mem->to_phy( pa ) ) | flags );
			if ( a == last )
				break;
			a += hsai::page_size;
			pa += hsai::page_size;
		}
		return true;
	}

	uint64 VirtualMemoryManager::vmalloc( PageTable &pt, uint64 old_sz, uint64 new_sz, bool executable )
	{
		void *mem;

		if ( new_sz < old_sz )
			return old_sz;

		old_sz = hsai::page_round_up( old_sz );
		for ( uint64 a = old_sz; a < new_sz; a += hsai::page_size )
		{
			// k_pmm.debug_print();
			mem = k_pmm.alloc_page();
			if ( mem == nullptr )
			{
				vmdealloc( pt, a, old_sz );
				return 0;
			}
			k_pmm.clear_page( mem );

			bool is_ok;
			if ( executable ) is_ok = map_code_pages( pt, a, hsai::page_size, ( ulong ) mem, true );
			else is_ok = map_data_pages( pt, a, hsai::page_size, ( ulong ) mem, true );

			if ( !is_ok )
			{
				k_pmm.free_pages( mem );
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

		if ( hsai::page_round_up( new_sz ) < hsai::page_round_up( old_sz ) )
		{
			int npages = ( hsai::page_round_up( old_sz ) - hsai::page_round_up( new_sz ) ) / hsai::page_size;
			vmunmap( pt, hsai::page_round_up( new_sz ), npages, true );
		}

		return new_sz;
	}

	int VirtualMemoryManager::uvmset( PageTable &pt, void * va, u8 n, int page_cnt )
	{
		if ( !hsai::is_page_align( ( ulong ) va ) )
		{
			hsai_warn( "uvmset : va(%p) is not page-align", va );
			return -1;
		}
		if ( page_cnt <= 0 )
			return 0;

		u64 fill = n;
		fill |= fill << 8;
		fill |= fill << 16;
		fill |= fill << 32;

		u8 * p = nullptr;
		int fill_cnt = hsai::page_size / sizeof( fill );
		char * vpa = ( char * ) va;
		for ( ; page_cnt > 0; page_cnt--, vpa += hsai::page_size )
		{
			p = ( u8 * ) hsai::k_mem->to_vir( pt.walk_addr( ( ulong ) vpa ) );
			if ( p == nullptr )
			{
				hsai_error( "uvmset : walk va(%p) fail!", vpa );
				return -2;
			}
			u64 * fp = ( u64 * ) p;
			for ( int i = 0; i < fill_cnt; ++i )
				fp[ i ] = fill;
		}
		return 0;
	}

	int VirtualMemoryManager::copy_in( PageTable &pt, void *dst, uint64 src_va, uint64 len )
	{
		uint64 n, va, pa;
		char *p_dst = ( char * ) dst;

		while ( len > 0 )
		{
			va = hsai::page_round_down( src_va );
			pa = ( uint64 ) pt.walk_addr( va );
			pa = hsai::k_mem->to_vir( pa );
			if ( pa == 0 )
				return -1;
			n = hsai::page_size - ( src_va - va );
			if ( n > len )
				n = len;
			memmove( ( void * ) p_dst, ( const void * ) ( pa + ( src_va - va ) ), n );

			len -= n;
			p_dst += n;
			src_va = va + hsai::page_size;
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
			va = hsai::page_round_down( src_va );
			pa = ( uint64 ) pt.walk_addr( va );
			pa = hsai::k_mem->to_vir( pa );
			if ( pa == 0 )
				return -1;
			n = hsai::page_size - ( src_va - va );
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

			src_va = va + hsai::page_size;
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

	int VirtualMemoryManager::copy_str_in( PageTable &pt, eastl::string &dst, uint64 src_va, uint64 max )
	{
		uint64 n, va, pa;
		int got_null = 0;

		while ( got_null == 0 && max > 0 )
		{
			va = hsai::page_round_down( src_va );
			pa = ( uint64 ) pt.walk_addr( va );
			pa = hsai::k_mem->to_vir( pa );

			if ( pa == 0 )
				return -1;
			n = hsai::page_size - ( src_va - va );
			if ( n > max )
				n = max;

			char *p = ( char * ) ( pa + ( src_va - va ) );
			while ( n > 0 )
			{
				if ( *p == '\0' )
				{
					got_null = 1;
					break;
				}
				else
				{
					dst.push_back( *p );
				}
				--n;
				--max;
				p++;
			}

			src_va = va + hsai::page_size;
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
			pm::Pcb *p = ( pm::Pcb * ) hsai::get_cur_proc();
			mm::PageTable *pt = p->get_pagetable();
			return copy_in( *pt, dst, src, len );
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

		if ( oldshm & 0xfff || newshm & 0xfff || newshm < sz || oldshm >( vm_trap_frame - 64 * 2 * hsai::page_size ) )
		{
			log_panic( "allocshm: bad parameters" );
			return 0;
		}
		a = newshm;
		for ( int i = 0; a < oldshm; a += hsai::page_size, i++ )
		{
			mem = k_pmm.alloc_page();
			if ( mem == nullptr )
			{
				log_panic( "allocshm: no memory" );
				deallocshm( pt, newshm, a );
				return 0;
			}
			map_data_pages( pt, a, hsai::page_size, uint64( phyaddr[ i ] ), true );
			phyaddr[ i ] = mem;
			printf( "allocshm: %p => %p\n", a, phyaddr[ i ] );
		}
		return newshm;
	}

	uint64 VirtualMemoryManager::mapshm( PageTable &pt, uint64 oldshm, uint64 newshm, uint sz, void **phyaddr )
	{
		uint64 a;
		if ( oldshm & 0xfff || newshm & 0xfff || newshm < sz || oldshm >( vm_trap_frame - 64 * 2 * hsai::page_size ) )
		{
			log_panic( "mapshm: bad parameters when shmmap" );
			return 0;
		}
		a = newshm;
		for ( int i = 0; a < oldshm; a += hsai::page_size, i++ )
		{
			map_data_pages( pt, a, hsai::page_size, uint64( phyaddr[ i ] ), true );
			printf( "mapshm: %p => %p\n", a, phyaddr[ i ] );
		}
		return newshm;
	}

	uint64 VirtualMemoryManager::deallocshm( PageTable &pt, uint64 oldshm, uint64 newshm )
	{
		if ( newshm <= oldshm )
			return oldshm;

		if ( hsai::page_round_up( newshm ) > hsai::page_round_up( oldshm ) )
		{
			int npages = hsai::page_round_up( newshm ) - hsai::page_round_up( oldshm ) / hsai::page_size;
			vmunmap( pt, hsai::page_round_up( oldshm ), npages, 0 );
		}
		return oldshm;
	}

	int VirtualMemoryManager::copyout( PageTable &pt, uint64 va, const void *p, uint64 len )
	{
		uint64 n, a, pa;

		while ( len > 0 )
		{
			a = hsai::page_round_down( va );
			hsai::Pte pte = pt.walk( a, 0 );
			pa = pte.to_pa();
			pa = hsai::k_mem->to_vir( pa );
			if ( pa == 0 )
				return -1;
			n = hsai::page_size - ( va - a );
			if ( n > len )
				n = len;
			memmove( ( void * ) ( hsai::k_mem->to_vir( pa + ( va - a ) ) ), p, n );

			len -= n;
			p = ( char* ) p + n;
			va = a + hsai::page_size;
		}
		return 0;
	}

	void VirtualMemoryManager::vmunmap( PageTable &pt, uint64 va, uint64 npages, int do_free )
	{
		uint64 a;
		hsai::Pte pte;

		if ( ( va % hsai::page_size ) != 0 )
			log_panic( "vmunmap: not aligned" );

		for ( a = va; a < va + npages * hsai::page_size; a += hsai::page_size )
		{
			if ( ( pte = pt.walk( a, 0 ) ).is_null() )
				log_panic( "vmunmap: walk" );
			if ( !pte.is_valid() )
				log_panic( "vmunmap: not mapped" );
			if ( pte.is_dir_page() )
				log_panic( "vmunmap: not a leaf" );
			if ( do_free )
			{
				k_pmm.free_pages( ( void* ) hsai::k_mem->to_vir( pte.to_pa() ) );
			}
			pte.clear_data();
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
		hsai::Pte pte;
		uint64 pa, va;
		uint64 flags;
		void *mem;

		for ( va = 0; va < size; va += hsai::page_size )
		{
			if ( ( pte = old_pt.walk( va, 0 ) ).is_null() )
				log_panic( "uvmcopy: pte should exist" );
			if ( !pte.is_present() )
				log_panic( "uvmcopy: page not present" );
			pa = ( uint64 ) pte.to_pa();
			pa = hsai::k_mem->to_vir( pa );
			flags = pte.get_flags();
			if ( ( mem = mm::k_pmm.alloc_page() ) == nullptr )
			{
				vmunmap( new_pt, 0, va / hsai::page_size, 1 );
				return -1;
			}
			memmove( mem, ( const char * ) pa, hsai::page_size );
			if ( map_pages( new_pt, va, hsai::page_size, ( uint64 ) mem, flags ) == false )
			{
				mm::k_pmm.free_pages( mem );
				vmunmap( new_pt, 0, va / hsai::page_size, 1 );
				return -1;
			}
		}
		return 0;
	}

	void VirtualMemoryManager::vmfree( PageTable &pt, uint64 sz )
	{
		if ( sz > 0 )
			vmunmap( pt, 0, hsai::page_round_up( sz ) / hsai::page_size, 1 );
		pt.freewalk();
	}

	void VirtualMemoryManager::uvmclear( PageTable &pt, uint64 va )
	{
		hsai::Pte pte = pt.walk( va, 0 );
		if ( pte.is_valid() )
			pte.set_super_plv();
	}

	uint64 VirtualMemoryManager::uvmalloc( PageTable &pt, uint64 oldsz, uint64 newsz )
	{
		uint64 a;
		uint64 pa;

		if ( newsz < oldsz )  // shrink, not here
			return oldsz;
		a = hsai::page_round_up( oldsz ); // start from the next page

		for ( a = oldsz; a < newsz; a += hsai::page_size )
		{
			pa = ( uint64 ) mm::k_pmm.alloc_page();
			if ( pa == 0 )
			{
				vmfree( pt, oldsz );
				return 0;
			}
			if ( !map_data_pages( pt, a, hsai::page_size, pa, true ) )
			{
				k_pmm.free_pages( ( void * ) pa );
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
		ulong nsz = hsai::page_round_up( newsz );
		ulong osz = hsai::page_round_up( oldsz );
		if ( nsz < osz )
			vmunmap( pt, nsz, ( osz - nsz ) / hsai::page_size, true );
		return newsz;
	}


}