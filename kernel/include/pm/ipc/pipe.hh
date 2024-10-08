//
// Created by Li Shuang ( pseudonym ) on 2024-05-29 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include <smp/spin_lock.hh>

#include <EASTL/queue.h>

namespace fs{

	class File;
	class pipe_file;
	
}
namespace pm
{
	class ProcessManager;

	namespace ipc
	{
		constexpr uint pipe_size = 1024;

		class Pipe
		{
			friend ProcessManager;
		private:
			hsai::SpinLock _lock;
			eastl::queue<uint8> _data;
			bool _read_is_open;
			bool _write_is_open;
			uint8 _read_sleep;
			uint8 _write_sleep;

		public:
			Pipe()
				: _read_is_open( false )
				, _write_is_open( false )
			{
				_lock.init( "pipe" );
			};
			bool read_is_open() { return _read_is_open; }
			bool write_is_open() { return _write_is_open; }

			int write( uint64 addr, int n );
			int write_in_kernel( uint64 addr, int n );

			int read( uint64 addr, int n );

			int alloc( fs::pipe_file * &f0, fs::pipe_file * &f1);

			void close( bool is_write );

		};

	} // namespace ipc
	
} // namespace pm
