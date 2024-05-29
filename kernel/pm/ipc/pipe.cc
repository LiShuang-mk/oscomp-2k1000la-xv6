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
					mm::PageTable pt = pr->get_pagetable();
					if ( mm::k_vmm.copy_in( pt, &ch, addr + i, 1 ) == -1 )
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
				mm::PageTable pt = pr->get_pagetable();
				if ( mm::k_vmm.copyout( pt, addr, &ch, 1 ) == -1 )
					break;
			}
			k_pm.wakeup( &_write_sleep ); //DOC: piperead-wakeup
			_lock.release();
			return i;
		}

	} // namespace ips

} // namespace pm
