//
// Created by Li Shuang ( pseudonym ) on 2024-05-20 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"
#include "klib/function.hh"

namespace dev
{
	constexpr uint max_major_dev_num = 128;
	constexpr uint dev_console_num = 1;

	using DeviceOperator = std::function<int( int, uint64, uint64 )>;

	struct DevOps
	{
		DeviceOperator read;
		DeviceOperator write;
	};


	class DeviceManager
	{
	private:
		DevOps _dev_ops[ max_major_dev_num ];

	public:
		void init();
		
		int register_device(
			uint dev_num,
			DeviceOperator dev_read,
			DeviceOperator dev_write
		);

		int read_device( uint dev_num, int mode, uint64 addr, uint64 n );
		int write_device( uint dev_num, int mode, uint64 addr, uint64 n );

	};

	extern DeviceManager k_dm;

} // namespace dev
