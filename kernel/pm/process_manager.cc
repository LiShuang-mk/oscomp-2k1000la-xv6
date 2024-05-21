//
// Created by Li shuang ( pseudonym ) on 2024-03-29 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hal/cpu.hh"
#include "pm/process_manager.hh"
#include "pm/process.hh"
#include "pm/trap_frame.hh"
#include "pm/scheduler.hh"
#include "mm/memlayout.hh"
#include "mm/physical_memory_manager.hh"
#include "mm/virtual_memory_manager.hh"
#include "im/trap_wrapper.hh"
#include "fs/fat/fat32_dir_entry.hh"
#include "im/exception_manager.hh"
#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <EASTL/map.h>
#include <EASTL/hash_map.h>
#include "klib/common.hh"
#include "fs/elf.hh"
#include "fs/fat/fat32_file_system.hh"
#include "hal/qemu_ls2k.hh"
#include "fs/file.hh"
#include "fs/dev/console.hh"
#include "fs/device.hh"
extern "C" {
	extern uint64 _start_u_init;
	extern uint64 _end_u_init;
	extern uint64 _u_init_stks;
	extern uint64 _u_init_stke;
	extern uint64 _u_init_txts;
	extern uint64 _u_init_txte;
	extern uint64 _u_init_dats;
	extern uint64 _u_init_date;
	extern int init_main( void );

	void _wrp_fork_ret( void )
	{
		pm::k_pm.fork_ret();
	}
}



namespace pm
{
	ProcessManager k_pm;

	void ProcessManager::init( const char *pid_lock_name, const char *wait_lock_name )
	{
		_pid_lock.init( pid_lock_name );
		_wait_lock.init( wait_lock_name );
		for ( uint i = 0; i < num_process; ++i )
		{
			Pcb &p = k_proc_pool[ i ];
			p.init( "pcb", i );
		}
		_cur_pid = 0;
	}

	Pcb *ProcessManager::get_cur_pcb()
	{
		loongarch::Cpu::push_intr_off();
		loongarch::Cpu *c_cpu = loongarch::Cpu::get_cpu();
		pm::Pcb *pcb = c_cpu->get_cur_proc();
		loongarch::Cpu::pop_intr_off();
		return pcb;
	}

	void ProcessManager::alloc_pid( Pcb *p )
	{
		_pid_lock.acquire();
		p->_pid = _cur_pid;
		_cur_pid++;
		_pid_lock.release();
	}

	Pcb *ProcessManager::alloc_proc()
	{
		Pcb *p;
		for ( p = k_proc_pool; p < &k_proc_pool[ num_process ]; p++ )
		{
			p->_lock.acquire();
			if ( p->_state == ProcState::unused )
			{
				pm::k_pm.alloc_pid( p );
				p->_state = ProcState::used;
				p->_slot = default_proc_slot;
				p->_priority = default_proc_prio;

				//p->_shm = mm::vml::vm_trap_frame - 64 * 2 * mm::PageEnum::pg_size;
				//p->_shmkeymask = 0;
				//pm::k_pm.set_vma( p );

				if ( ( p->_trapframe = ( TrapFrame* ) mm::k_pmm.alloc_page() ) == nullptr )
				{
					freeproc( p );
					p->_lock.release();
					return nullptr;
				}

				_proc_create_vm( p );
				if ( p->_pt.get_base() == 0 )
				{
					freeproc( p );
					p->_lock.release();
					return nullptr;
				}

				p->_mqmask = 0;

				memset( &p->_context, 0, sizeof( p->_context ) );

				p->_context.ra = ( uint64 ) _wrp_fork_ret;

				p->_context.sp = p->_kstack + mm::pg_size;

				p->_lock.release();

				return p;
			}
			else
			{
				p->_lock.release();
			}
		}
		return nullptr;
	}


	void ProcessManager::set_shm( Pcb * p )
	{
		k_pm._pid_lock.acquire();
		p->_lock.acquire();
		p->_shm = mm::vml::vm_trap_frame - 64 * 2 * mm::PageEnum::pg_size;
		p->_shmkeymask = 0;
		p->_lock.release();
		k_pm._pid_lock.release();
	}

	void ProcessManager::set_vma( Pcb * p )
	{
		for ( int i = 1; i < 10; i++ )
		{
			p->vm[ i ]->next = -1;
			p->vm[ i ]->length = 0;
		}
		p->vm[ 0 ]->next = 1;
	}

	void ProcessManager::freeproc( Pcb * p )
	{
		if ( p->_trapframe )
			mm::k_pmm.free_page( ( void * ) p->_trapframe );
		p->_trapframe = 0;
		if ( !p->_pt.is_null() )
		{
			mm::k_vmm.vmunmap( p->_pt, mm::PageEnum::pg_size, 1, 0 );
			mm::k_vmm.vmfree( p->_pt, p->_sz );
		}
		p->_pt.set_base( 0 );
		p->_sz = 0;
		p->_pid = 0;
		p->parent = 0;
		p->_chan = 0;
		p->_name[ 0 ] = 0;
		p->_killed = 0;
		p->_xstate = 0;
		p->_state = ProcState::unused;
	}


	void ProcessManager::user_init()
	{
		static int inited = 0;
		static char user_init_proc_name[] = "user init";
		if ( inited != 0 )
		{
			log_warn( "re-init user." );
			return;
		}

		Pcb * p = alloc_proc();
		assert( p != nullptr, "pm: alloc proc fail while user init." );

		_init_proc = p;
		p->_lock.acquire();

		for ( uint i = 0; i < sizeof( user_init_proc_name ); ++i )
		{
			p->_name[ i ] = user_init_proc_name[ i ];
		}

		p->_sz = ( uint64 ) &_end_u_init - ( uint64 ) &_start_u_init;
		//p->_sz = 0;
		
		// map user init stack
		mm::k_vmm.map_pages(
			p->_pt,
			0,
			( uint64 ) &_u_init_stke - ( uint64 ) &_u_init_stks,
			( uint64 ) &_u_init_stks,
			loongarch::PteEnum::presence_m |
			loongarch::PteEnum::writable_m |
			loongarch::PteEnum::dirty_m |
			( 0x3 << loongarch::PteEnum::plv_s ) |
			( loongarch::mat_cc << loongarch::PteEnum::mat_s )
		);

		// map user init function
		mm::k_vmm.map_pages(
			p->_pt,
			( uint64 ) &_u_init_txts - ( uint64 ) &_start_u_init,
			( uint64 ) &_u_init_txte - ( uint64 ) &_u_init_txts,
			( uint64 ) &_u_init_txts ,
			loongarch::PteEnum::presence_m |
			loongarch::PteEnum::writable_m |
			( 0x3 << loongarch::PteEnum::plv_s ) |
			( loongarch::mat_cc << loongarch::PteEnum::mat_s )
		);

		// map user init data
		mm::k_vmm.map_pages(
			p->_pt,
			( uint64 ) &_u_init_dats - ( uint64 ) &_start_u_init,
			( uint64 ) &_u_init_date - ( uint64 ) &_u_init_dats,
			( uint64 ) &_u_init_dats,
			loongarch::PteEnum::presence_m |
			loongarch::PteEnum::writable_m |
			loongarch::PteEnum::dirty_m |
			( 0x3 << loongarch::PteEnum::plv_s ) |
			( loongarch::mat_cc << loongarch::PteEnum::mat_s )
		);


		p->_trapframe->era = ( uint64 ) &init_main - ( uint64 ) &_start_u_init;
		log_info( "user init: era = %p", p->_trapframe->era );
		p->_trapframe->sp = ( uint64 ) &_u_init_stke - ( uint64 ) &_start_u_init;
		log_info( "user init: sp  = %p", p->_trapframe->sp );

		fs::xv6_file *f = fs::k_file_table.alloc_file();
		assert( f != nullptr, "pm: alloc file fail while user init." );
		f->writable = 1;
		f->readable = 1;
		f->major = dev::dev_console_num;
		f->type = fs::xv6_file::FD_DEVICE;
		p->_ofile[ 1 ] = f;


		/// TODO:
		/// set p->cwd = "/"

		p->_state = ProcState::runnable;

		p->_lock.release();

		inited = 1;

		loongarch::Cpu::get_cpu()->set_cur_proc( p );

	}

	void ProcessManager::sche_proc( Pcb *p )
	{
		p->_slot--;
		if ( p->_slot == 0 )
		{
			p->_slot = default_proc_slot;
			k_scheduler.yield();
		}
	}

	int ProcessManager::fork()
	{
		int i, pid;
		Pcb *np;					// new proc
		Pcb *p = get_cur_pcb();		// current proc

		// Allocate process.
		if ( ( np = alloc_proc() ) == nullptr )
		{
			return -1;
		}

		np->_lock.acquire();

		// Copy user memory from parent to child.
		mm::PageTable curpt, newpt;
		curpt = p->get_pagetable();
		newpt = np->get_pagetable();
		if ( mm::k_vmm.vm_copy( curpt, newpt, p->_sz ) < 0 )
		{
			freeproc( np );
			np->_lock.release();
			return -1;
		}
		np->_sz = p->_sz;

		/// TODO: >> Share Memory Copy
		// shmaddcount( p->shmkeymask );
		// np->shm = p->shm;
		// np->shmkeymask = p->shmkeymask;
		// for ( i = 0; i < MAX_SHM_NUM; ++i )
		// {
		// 	if ( shmkeyused( i, np->shmkeymask ) )
		// 	{
		// 		np->shmva[ i ] = p->shmva[ i ];
		// 	}
		// }

		// copy saved user registers.
		*( np->get_trapframe() ) = *( p->get_trapframe() );

		// Cause fork to return 0 in the child.
		np->get_trapframe()->a0 = 0;

		/// TODO: >> Message Queue Copy
		// addmqcount( p->mqmask );
		// np->mqmask = p->mqmask;

		// increment reference counts on open file descriptors.
		for ( i = 0; i < ( int ) max_open_files; i++ )
			if ( p->_ofile[ i ] )
			{
				fs::k_file_table.dup( p->_ofile[ i ] );
				np->_ofile[ i ] = p->_ofile[ i ];
			}

		/// TODO: >> cwd inode ref-up
		// np->cwd = idup( p->cwd );

		strncpy( np->_name, p->_name, sizeof( p->_name ) );

		pid = np->_pid;

		np->_lock.release();

		_wait_lock.acquire();
		np->parent = p;
		_wait_lock.release();

		np->_lock.acquire();
		np->_state = ProcState::runnable;
		np->_lock.release();

		return pid;
	}

	void ProcessManager::fork_ret()
	{
		// Still holding p->lock from scheduler.
		loongarch::Cpu::get_cpu()->get_cur_proc()->_lock.release();

		im::k_em.user_trap_ret();
	}


	mm::PageTable ProcessManager::proc_pagetable( Pcb *p )
	{
		mm::PageTable pt;

		uint64 pa = ( uint64 ) mm::k_pmm.alloc_page();
		if ( pa == 0 )
			return pt;
		pt.set_base( pa );

		// if(!mm::k_vmm.map_pages(pt, mm::vm_trap_frame, mm::PageEnum::pg_size, (uint64)p->_trapframe,
		// 	( loongarch::PteEnum::presence_m ) |
		// 	( loongarch::PteEnum::writable_m ) |
		// 	( loongarch::PteEnum::plv_m ) |
		// 	( loongarch::PteEnum::mat_m ) |
		// 	( loongarch::PteEnum::dirty_m )))
		// {
		// 	mm::k_vmm.vmfree(pt, mm::PageEnum::pg_size);
		// 	return pt;
		// }
		return pt;
	}

	void ProcessManager::proc_freepagetable( mm::PageTable pt, uint64 sz )
	{
		mm::k_vmm.vmunmap( pt, mm::PageEnum::pg_size, 1, 0 );
		mm::k_vmm.vmfree( pt, sz );
	}

	int ProcessManager::exec( eastl::string path, eastl::vector<eastl::string> argv )
	{
		Pcb *proc = get_cur_pcb();
		uint64 sz = 0;
		uint64 sp;
		uint64 stackbase;
		uint64 argc;
		mm::PageTable pt;
		mm::PageTable pt_old;
		uint64 ustack[ MAXARG ];
		elf::elfhdr elf;
		elf::proghdr ph;
		fs::fat::Fat32DirInfo dir_;
		fs::fat::Fat32DirEntry *de;
		int i, off;
		if ( fs::fat::k_testcase_fs.get_root_dir()->find_sub_dir( path, dir_ )
			== fs::fat::Fat32DirEntryStatus::fat32de_init_fail )
		{
			log_error( "exec: cannot find file" );
			return -1;   // 拿到文件夹信息
		}
		if ( ( de = fs::fat::k_testcase_fs.get_dir_entry( dir_ ) ) == nullptr )
		{
			log_error( "exec: cannot find file" );
			return -1; 	 // 拿到文件信息
		}

		/// @todo check ELF header
		de->read_content( &elf, sizeof( elf ), 0 );

		if ( elf.magic != elf::elfEnum::ELF_MAGIC )  // check magicnum
		{
			log_error( "exec: not a valid ELF file" );
			return -1;
		}

		// create user pagetable for given process
		// if((pt = proc_pagetable(proc)).is_null()){
		// 	log_error("exec: cannot create pagetable");
		// 	return -1;
		// }
		
		for ( i = 0, off = elf.phoff; i < elf.phnum; i++, off += sizeof( ph ) )
		{
			de->read_content( &ph, sizeof( ph ), off );

			if ( ph.type != elf::elfEnum::ELF_PROG_LOAD )
				continue;
			if ( ph.memsz < ph.filesz )
			{
				log_error( "exec: memsz < filesz" );
				proc_freepagetable( proc->_pt, sz );
				return -1;
			}
			if ( ph.vaddr + ph.memsz < ph.vaddr )
			{
				log_error( "exec: vaddr + memsz < vaddr" );
				proc_freepagetable( proc->_pt, sz );
				return -1;
			}
			uint64 sz1;
			if ( ( sz1 = mm::k_vmm.vmalloc( proc->_pt, sz, ph.vaddr + ph.memsz ) ) == 0 )
			{
				log_error( "exec: uvmalloc" );
				proc_freepagetable( proc->_pt, sz );
				return -1;
			}
			sz = sz1;
			if ( ( ph.vaddr % mm::pg_size ) != 0 )
			{
				log_error( "exec: vaddr not aligned" );
				proc_freepagetable( proc->_pt, sz );
				return -1;
			}

			if ( load_seg( proc->_pt, ph.vaddr, de, ph.off, ph.filesz ) < 0 )
			{
				log_error( "exec: load_icode" );
				proc_freepagetable( proc->_pt, sz );
				return -1;
			}
		}

		proc = k_pm.get_cur_pcb();

		sz = mm::page_round_up( sz );

		//allocate two pages , the second is used for the user stack
		uint64 sz1;

		if ( ( sz1 = mm::k_vmm.uvmalloc( proc->_pt, sz, sz + 2 * mm::PageEnum::pg_size ) ) == 0 )
		{
			log_error( "exec: vmalloc when allocating stack" );
			proc_freepagetable( proc->_pt, sz );
			return -1;
		}

		sz = sz1;
		mm::k_vmm.uvmclear( proc->_pt, sz - 2 * mm::PageEnum::pg_size );
		sp = sz;
		stackbase = sp - mm::PageEnum::pg_size;

		//push argument strings, prepare rest of stack in ustack.
		for ( argc = 0; argc < argv.size(); argc++ )
		{
			if ( argc >= MAXARG )
			{
				proc_freepagetable( proc->_pt, sz );
				log_panic( "exec: too many arguments" );
				return -1;
			}

			sp -= argv[ argc ].length() + 1;
			sp -= sp % 16;
			if ( sp < stackbase )
			{
				proc_freepagetable( proc->_pt, sz );
				log_panic( "exec: sp < stackbase" );
				return -1;
			}
			if ( mm::k_vmm.copyout( proc->_pt, sp, argv[ argc ].c_str(), argv[ argc ].length() + 1 ) < 0 )
			{
				proc_freepagetable( proc->_pt, sz );
				log_panic( "exec: copyout" );
				return -1;
			}

			ustack[ argc ] = sp;
		}
		ustack[ argc ] = 0;

		// push array of argument pointers
		sp -= ( argc + 1 ) * sizeof( uint64 );
		sp -= sp % 16;

		if ( sp < stackbase )
		{
			proc_freepagetable( proc->_pt, sz );
			log_panic( "exec: sp < stackbase" );
			return -1;
		}
		if ( mm::k_vmm.copyout( proc->_pt, sp, ustack, ( argc + 1 ) * sizeof( uint64 ) ) < 0 )
		{
			proc_freepagetable( proc->_pt, sz );
			log_panic( "exec: copyout" );
			return -1;
		}

		// arguments to user main(argc, argv)
		// argc is returned via the system call return
		// value, which is in a0.
		proc->_trapframe->a1 = sp;

		// save program name for debugging.
		for ( uint i = 0; i < 16; i++ )
		{
			if ( i < path.size() )
				proc->_name[ i ] = path[ i ];
			else
				proc->_name[ i ] = 0;
		}

		// commit to the user image.
		proc->_sz = sz;
		proc->_trapframe->era = elf.entry;
		proc->_trapframe->sp = sp;
		proc->_state = ProcState::runnable;
		return argc;
	}

	int ProcessManager::load_seg( mm::PageTable &pt, uint64 va, fs::fat::Fat32DirEntry *de, uint offset, uint size )
	{	//好像没有机会返回 -1, pa失败的话会panic，de的read也没有返回值
		uint i, n;
		uint64 pa;

		for ( i = 0; i < size; i += mm::PageEnum::pg_size )
		{
			pa = ( uint64 ) pt.walk( va + i, 0 ).pa();
			if ( pa == 0 )
				log_panic( "load_seg: walk" );
			if ( size - i < mm::PageEnum::pg_size )
				n = size - i;
			else
				n = mm::PageEnum::pg_size;
			de->read_content( ( void * ) ( pa | loongarch::qemuls2k::dmwin::win_0), n, offset + i );
		}
		return 0;
	}


	int ProcessManager::wait( uint64 addr )
	{
		Pcb *p = k_pm.get_cur_pcb();
		int havekids, pid;
		Pcb *np = nullptr;

		_wait_lock.acquire();
		for ( ;;)
		{
			havekids = 0;
			for ( np = k_proc_pool; np < &k_proc_pool[ num_process ]; np++ )
			{
				if ( np->parent == p )
				{
					np->_lock.acquire();
					havekids = 1;

					if ( np->get_state() == ProcState::zombie )
					{
						pid = np->_pid;
						if ( addr != 0 && mm::k_vmm.copyout( p->_pt, addr, ( char * ) &pid, sizeof( pid ) ) < 0 )
						{
							np->_lock.release();
							_wait_lock.release();
							return -1;
						}
						/// @todo release shm

						k_pm.freeproc( np );
						np->_lock.release();
						_wait_lock.release();
						return pid;
					}
					np->_lock.release();
				}
			}

			if ( !havekids || p->_killed )
			{
				_wait_lock.release();
				return -1;
			}

			//wait children to exit
			sleep( p, &_wait_lock );
		}
	}

	void ProcessManager::exit( int state )
	{
		Pcb *p = get_cur_pcb();

		/// @todo close opened file, set proc's pwd

		_wait_lock.acquire();

		/// @todo give it's children to initproc

		if ( p->parent )
			wakeup( p->parent );

		p->_lock.acquire();
		p->_xstate = state;
		p->_state = ProcState::zombie;

		_wait_lock.release();

		k_scheduler.call_sched(); // jump to schedular, never return
		log_panic( "zombie exit" );

	}

	void ProcessManager::wakeup( void *chan )
	{
		Pcb *p;
		for ( p = k_proc_pool; p < &k_proc_pool[ num_process ]; p++ )
		{
			if ( p != k_pm.get_cur_pcb() )
			{
				p->_lock.acquire();
				if ( p->_state == ProcState::sleeping && p->_chan == chan )
				{
					p->_state = ProcState::runnable;
				}
				p->_lock.release();
			}
		}
	}

	void ProcessManager::sleep( void *chan, smp::Lock *lock )
	{
		Pcb *proc = k_pm.get_cur_pcb();

		//get the lock to release and change it's state to scheduler
		proc->_lock.acquire();
		lock->release();

		proc->_chan = chan;
		proc->_state = ProcState::sleeping;

		k_scheduler.call_sched();
		proc->_chan = 0;

		proc->_lock.release();
		lock->acquire();
	}

// ---------------- private helper functions ----------------

	void ProcessManager::_proc_create_vm( Pcb * p )
	{
		mm::PageTable pt;

		pt = mm::k_vmm.vm_create();
		if ( pt.get_base() == 0 )
		{
			return;
		}

		if ( mm::k_vmm.map_pages(
			pt,
			mm::vml::vm_trap_frame,
			mm::pg_size,
			( uint64 ) ( p->_trapframe ),
			loongarch::PteEnum::nx_m |
			loongarch::PteEnum::presence_m |
			loongarch::PteEnum::writable_m |
			loongarch::PteEnum::dirty_m |
			( loongarch::mat_cc << loongarch::PteEnum::mat_s )
		) == false )
		{
			mm::k_vmm.vmfree( pt, 0 );
			return;
		}

		p->_pt = pt;
	}

// ---------------- test function ----------------

	void ProcessManager::vectortest()
	{
		eastl::vector<int> v;

		// 测试 push_back
		v.push_back( 1 );
		v.push_back( 2 );
		v.push_back( 3 );
		v.push_back( 4 );

		// 测试 size
		log_trace( "vector size: %d\n", v.size() );

		// 测试 capacity
		log_trace( "vector capacity: %d\n", v.capacity() );

		// 测试 empty
		log_trace( "vector is empty: %d\n", v.empty() );

		// 测试 at
		log_trace( "vector at 2: %d\n", v.at( 2 ) );

		// 测试 front
		log_trace( "vector front: %d\n", v.front() );

		// 测试 back
		log_trace( "vector back: %d\n", v.back() );

		// 测试 insert
		v.insert( v.begin() + 2, 5 );
		log_trace( "vector after insert: " );
		for ( auto i = v.begin(); i != v.end(); i++ )
		{
			log_trace( "%d ", *i );
		}
		log_trace( "\n" );

		// 测试 erase
		v.erase( v.begin() + 2 );
		log_trace( "vector after erase: " );
		for ( auto i = v.begin(); i != v.end(); i++ )
		{
			log_trace( "%d ", *i );
		}
		log_trace( "\n" );

		// 测试 swap
		eastl::vector<int> v2;
		v2.push_back( 6 );
		v2.push_back( 7 );
		v.swap( v2 );
		log_trace( "vector after swap: " );
		for ( auto i = v.begin(); i != v.end(); i++ )
		{
			log_trace( "%d ", *i );
		}
		log_trace( "\n" );

		// 测试 resize
		v.resize( 5, 8 );
		log_trace( "vector after resize: " );
		for ( auto i = v.begin(); i != v.end(); i++ )
		{
			log_trace( "%d ", *i );
		}
		log_trace( "\n" );

		// 测试 reserve
		v.reserve( 10 );
		log_trace( "vector capacity after reserve: %d\n", v.capacity() );

		// 测试 clear
		v.clear();
		log_trace( "vector size after clear: %d\n", v.size() );
	}

	void ProcessManager::stringtest()
	{
		eastl::string s;

		// 测试赋值
		s = "hello world";
		log_trace( "string: %s\n", s.c_str() );

		// 测试 size 和 length
		log_trace( "string size: %d\n", s.size() );
		log_trace( "string length: %d\n", s.length() );

		// 测试 empty
		log_trace( "string is empty: %d\n", s.empty() );

		// 测试 append
		s.append( " EASTL" );
		log_trace( "string after append: %s\n", s.c_str() );

		// 测试 insert
		s.insert( 5, ", dear" );
		log_trace( "string after insert: %s\n", s.c_str() );

		// 测试 erase
		s.erase( 5, 6 );
		log_trace( "string after erase: %s\n", s.c_str() );

		// 测试 replace
		s.replace( 6, 5, "EASTL" );
		log_trace( "string after replace: %s\n", s.c_str() );

		// 测试 substr
		eastl::string sub = s.substr( 6, 5 );
		log_trace( "substring: %s\n", sub.c_str() );

		// 测试 find
		size_t pos = s.find( "EASTL" );
		log_trace( "find EASTL at: %d\n", pos );

		// 测试 rfind
		pos = s.rfind( 'l' );
		log_trace( "rfind 'l' at: %d\n", pos );

		// 测试 compare
		int cmp = s.compare( "hello EASTL" );
		log_trace( "compare with 'hello EASTL': %d\n", cmp );
	}

	void ProcessManager::maptest()
	{
		eastl::map<int, int> m;

		// 测试 insert
		m.insert( eastl::make_pair( 1, 2 ) );
		m.insert( eastl::make_pair( 3, 4 ) );
		m.insert( eastl::make_pair( 5, 6 ) );

		// 测试 size
		log_trace( "map size: %d\n", m.size() );

		// 测试 empty
		log_trace( "map is empty: %d\n", m.empty() );

		// 测试 at
		log_trace( "map at 3: %d\n", m.at( 3 ) );

		// 测试 operator[]
		log_trace( "map[5]: %d\n", m[ 5 ] );

		// 测试 find
		auto it = m.find( 3 );
		log_trace( "find 3: %d\n", it->second );

		// 测试 erase
		m.erase( 3 );
		log_trace( "map size after erase: %d\n", m.size() );

		// 测试 clear
		m.clear();
		log_trace( "map size after clear: %d\n", m.size() );
	}

	void ProcessManager::hashtest()
	{
		eastl::hash_map<int, int> m;

		// 测试 insert
		m.insert( eastl::make_pair( 1, 2 ) );
		m.insert( eastl::make_pair( 3, 4 ) );
		m.insert( eastl::make_pair( 5, 6 ) );

		// 测试 size
		log_trace( "hash_map size: %d\n", m.size() );

		// 测试 empty
		log_trace( "hash_map is empty: %d\n", m.empty() );

		// 测试 at
		log_trace( "hash_map at 3: %d\n", m.at( 3 ) );

		// 测试 operator[]
		log_trace( "hash_map[5]: %d\n", m[ 5 ] );

		// 测试 find
		auto it = m.find( 3 );
		log_trace( "find 3: %d\n", it->second );

		// 测试 erase
		m.erase( 3 );
		log_trace( "hash_map size after erase: %d\n", m.size() );

		// 测试 clear
		m.clear();
		log_trace( "hash_map size after clear: %d\n", m.size() );
	}
}