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

}
namespace pm
{
	class ProcessManager;

	namespace ipc
	{
		constexpr uint pipe_size = 512;

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

			int write( uint64 addr, int n );

			int read( uint64, int n );

			int alloc( fs::File * &f0, fs::File * &f1);

			void close( bool is_write );

		};

	} // namespace ipc
	
} // namespace pm
