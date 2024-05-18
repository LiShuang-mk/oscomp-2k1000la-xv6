//
// Created by Li shuang ( pseudonym ) on 2024-03-29 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "pm/process_manager.hh"
#include "pm/process.hh"
#include "hal/cpu.hh"
#include "mm/memlayout.hh"
#include "mm/physical_memory_manager.hh"
#include "mm/virtual_memory_manager.hh"
#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <EASTL/map.h>
#include <EASTL/hash_map.h>
#include "klib/common.hh"
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

	bool ProcessManager::change_state( Pcb *p, ProcState state )
	{
		_pid_lock.acquire();
		p->_lock.acquire();
		if ( p->_state == ProcState::unused )
		{
			p->_lock.release();
			_pid_lock.release();
			return false;
		}
		p->_state = state;
		p->_lock.release();
		_pid_lock.release();
		return true;
	}

	void ProcessManager::alloc_pid(Pcb *p)
	{
		_pid_lock.acquire();
		p->_lock.acquire();
		p->_pid = _cur_pid++;
		p->_lock.release();
		_pid_lock.release();
	}

	Pcb *ProcessManager::alloc_proc()
	{
		Pcb *p;
		for(p = k_proc_pool;p<&k_proc_pool[ num_process ];p++)
		{
			p->_lock.acquire();
			if(p->get_state() == ProcState::unused)
			{
				pm::k_pm.change_state(p,ProcState::runnable);
				pm::k_pm.alloc_pid(p);
				pm::k_pm.set_priority(p, 19);
				pm::k_pm.set_slot(p, 10);
				pm::k_pm.set_shm(p);
				pm::k_pm.set_vma(p);
				return p;
			}
		}
		return p;
	}

	void ProcessManager::set_priority(Pcb *p, int priority)
	{
		k_pm._pid_lock.acquire();
		p->_lock.acquire();
		p->_priority = priority;
		p->_lock.release();
		k_pm._pid_lock.release();
	}
	
	void ProcessManager::set_slot(Pcb *p, int slot)
	{
		k_pm._pid_lock.acquire();
		p->_lock.acquire();
		p->_slot = slot;
		p->_lock.release();
		k_pm._pid_lock.release();
	}

	void ProcessManager::set_shm(Pcb *p)
	{
		k_pm._pid_lock.acquire();
		p->_lock.acquire();
		p->_shm = mm::vml::vm_trap_frame - 64 * 2 * mm::PageEnum::pg_size;
		p->_shmkeymask = 0;
		p->_lock.release();
		k_pm._pid_lock.release();
	}

	void ProcessManager::set_vma(Pcb *p)
	{
		k_pm._pid_lock.acquire();
		p->_lock.acquire();
		for(int i=1;i<10;i++)
		{
			p->vm[i]->next = -1;
			p->vm[i]->length = 0;
		}
		p->vm[0]->next = 1;
		p->_lock.release();
		k_pm._pid_lock.release();
	}

	int ProcessManager::set_trapframe(Pcb *p)
	{
		k_pm._pid_lock.acquire();
		p->_lock.acquire();
		if((p->_trapframe = (struct TrapFrame *)mm::k_pmm.alloc_page()) == nullptr)
		{
			//k_pm.freeproc(p);
			p->_lock.release();
			k_pm._pid_lock.release();
			return -1;
		}
		return 0;
	}

	void ProcessManager::freeproc(Pcb *p)
	{
		if(p->_trapframe)
			mm::k_pmm.free_page((void *)p->_trapframe);	
		p->_trapframe = 0;
		if(!p->_pt.is_null())
		{
			mm::k_vmm.vmunmap(p->_pt,mm::PageEnum::pg_size,1,0);
			mm::k_vmm.vmfree(p->_pt,p->_sz);
		}
		p->_pt.set_base(0);
		p->_sz = 0;
		p->_pid = 0;
		p-> parent = 0;
		p->_chan = 0;
		p->_name[0] = 0;
		p->_killed = 0;
		p->_xstate = 0;
		p->_state = ProcState::unused;
	}

	void ProcessManager::exit( int )
	{
		
	}

	void ProcessManager::vectortest()
	{
		eastl::vector<int> v;

		// 测试 push_back
		v.push_back(1);
		v.push_back(2);
		v.push_back(3);
		v.push_back(4);

		// 测试 size
		log_trace("vector size: %d\n", v.size());

		// 测试 capacity
		log_trace("vector capacity: %d\n", v.capacity());

		// 测试 empty
		log_trace("vector is empty: %d\n", v.empty());

		// 测试 at
		log_trace("vector at 2: %d\n", v.at(2));

		// 测试 front
		log_trace("vector front: %d\n", v.front());

		// 测试 back
		log_trace("vector back: %d\n", v.back());

		// 测试 insert
		v.insert(v.begin() + 2, 5);
		log_trace("vector after insert: ");
		for(auto i = v.begin(); i != v.end(); i++)
		{
			log_trace("%d ", *i);
		}
		log_trace("\n");

		// 测试 erase
		v.erase(v.begin() + 2);
		log_trace("vector after erase: ");
		for(auto i = v.begin(); i != v.end(); i++)
		{
			log_trace("%d ", *i);
		}
		log_trace("\n");

		// 测试 swap
		eastl::vector<int> v2;
		v2.push_back(6);
		v2.push_back(7);
		v.swap(v2);
		log_trace("vector after swap: ");
		for(auto i = v.begin(); i != v.end(); i++)
		{
			log_trace("%d ", *i);
		}
		log_trace("\n");

		// 测试 resize
		v.resize(5, 8);
		log_trace("vector after resize: ");
		for(auto i = v.begin(); i != v.end(); i++)
		{
			log_trace("%d ", *i);
		}
		log_trace("\n");

		// 测试 reserve
		v.reserve(10);
		log_trace("vector capacity after reserve: %d\n", v.capacity());

		// 测试 clear
		v.clear();
		log_trace("vector size after clear: %d\n", v.size());
	}

	void ProcessManager::stringtest()
	{
		eastl::string s;

		// 测试赋值
		s = "hello world";
		log_trace("string: %s\n", s.c_str());

		// 测试 size 和 length
		log_trace("string size: %d\n", s.size());
		log_trace("string length: %d\n", s.length());

		// 测试 empty
		log_trace("string is empty: %d\n", s.empty());

		// 测试 append
		s.append(" EASTL");
		log_trace("string after append: %s\n", s.c_str());

		// 测试 insert
		s.insert(5, ", dear");
		log_trace("string after insert: %s\n", s.c_str());

		// 测试 erase
		s.erase(5, 6);
		log_trace("string after erase: %s\n", s.c_str());

		// 测试 replace
		s.replace(6, 5, "EASTL");
		log_trace("string after replace: %s\n", s.c_str());

		// 测试 substr
		eastl::string sub = s.substr(6, 5);
		log_trace("substring: %s\n", sub.c_str());

		// 测试 find
		size_t pos = s.find("EASTL");
		log_trace("find EASTL at: %d\n", pos);

		// 测试 rfind
		pos = s.rfind('l');
		log_trace("rfind 'l' at: %d\n", pos);

		// 测试 compare
		int cmp = s.compare("hello EASTL");
		log_trace("compare with 'hello EASTL': %d\n", cmp);
	}

	void ProcessManager::maptest()
	{
		eastl::map<int, int> m;

		// 测试 insert
		m.insert(eastl::make_pair(1, 2));
		m.insert(eastl::make_pair(3, 4));
		m.insert(eastl::make_pair(5, 6));

		// 测试 size
		log_trace("map size: %d\n", m.size());

		// 测试 empty
		log_trace("map is empty: %d\n", m.empty());

		// 测试 at
		log_trace("map at 3: %d\n", m.at(3));

		// 测试 operator[]
		log_trace("map[5]: %d\n", m[5]);

		// 测试 find
		auto it = m.find(3);
		log_trace("find 3: %d\n", it->second);

		// 测试 erase
		m.erase(3);
		log_trace("map size after erase: %d\n", m.size());

		// 测试 clear
		m.clear();
		log_trace("map size after clear: %d\n", m.size());
	}

	void ProcessManager::hashtest()
	{
		eastl::hash_map<int, int> m;

		// 测试 insert
		m.insert(eastl::make_pair(1, 2));
		m.insert(eastl::make_pair(3, 4));
		m.insert(eastl::make_pair(5, 6));

		// 测试 size
		log_trace("hash_map size: %d\n", m.size());

		// 测试 empty
		log_trace("hash_map is empty: %d\n", m.empty());

		// 测试 at
		log_trace("hash_map at 3: %d\n", m.at(3));

		// 测试 operator[]
		log_trace("hash_map[5]: %d\n", m[5]);

		// 测试 find
		auto it = m.find(3);
		log_trace("find 3: %d\n", it->second);

		// 测试 erase
		m.erase(3);
		log_trace("hash_map size after erase: %d\n", m.size());

		// 测试 clear
		m.clear();
		log_trace("hash_map size after clear: %d\n", m.size());
	}
}