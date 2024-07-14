//
// Created by Li Shuang ( pseudonym ) on 2024-05-29 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "pm/ipc/pipe.hh"
#include "pm/process_manager.hh"

#include "mm/virtual_memory_manager.hh"

#include "fs/file.hh"
namespace pm
{
	namespace ipc
	{
		int Pipe::write( uint64 addr, int n )
		{
			int i = 0;
			Pcb * pr = k_pm.get_cur_pcb();

			_lock.acquire();
			while ( i < n )
			{
				if ( !_read_is_open || pr->is_killed() )
				{
					_lock.release();
					return -1;
				}
				if ( _data.size() >= pipe_size )
				{ //DOC: pipewrite-full
					k_pm.wakeup( &_read_sleep );
					k_pm.sleep( &_write_sleep, &_lock );
				}
				else
				{
					char ch;
					mm::PageTable *pt = pr->get_pagetable();
					if ( mm::k_vmm.copy_in( *pt, &ch, addr + i, 1 ) == -1 )
						break;
					_data.push( ch );
					i++;
				}
			}
			k_pm.wakeup( &_read_sleep );
			_lock.release();

			return i;
		}

		int Pipe::read( uint64 addr, int n )
		{
			int i;
			Pcb *pr = k_pm.get_cur_pcb();
			char ch;

			_lock.acquire();
			while ( _data.size() == 0 && _write_is_open )
			{  //DOC: pipe-empty
				if ( pr->is_killed() )
				{
					_lock.release();
					return -1;
				}
				k_pm.sleep( &_read_sleep, &_lock ); //DOC: piperead-sleep
			}
			for ( i = 0; i < n; i++ )
			{  //DOC: piperead-copy
				if ( _data.size() == 0 )
					break;
				ch = _data.front();
				_data.pop();
				*( ( ( char * ) addr ) + i ) = ch;
			}
			k_pm.wakeup( &_write_sleep ); //DOC: piperead-wakeup
			_lock.release();
			return i;
		}

		int Pipe::alloc( fs::xv6_file * &f0, fs::xv6_file * &f1 )
		{
			if ( ( f0 = fs::k_file_table.alloc_file() ) == nullptr
				|| ( f1 = fs::k_file_table.alloc_file() ) == nullptr )
				return -1; // allocate file failed

			// init pipe
			_read_is_open = true;
			_write_is_open = true;
			_data = eastl::queue<uint8>();

			// set file
			f0->type = fs::xv6_file::FD_PIPE;
			f0->pipe = this;
			f0->readable = 1;
			f0->writable = 0;

			f1->pipe = this;
			f1->type = fs::xv6_file::FD_PIPE;
			f1->readable = 0;
			f1->writable = 1;

			return 0;
		}

		void Pipe::close( bool is_write )
		{
			_lock.acquire();
			if ( is_write )
			{
				_write_is_open = false;
				k_pm.wakeup( &_read_sleep );
			}
			else
			{
				_read_is_open = false;
				k_pm.wakeup( &_write_sleep );
			}

			if ( !_read_is_open && !_write_is_open )
			{
				_lock.release();
				delete this;
			}
			else
				_lock.release();
		}

	} // namespace ips

} // namespace pm
