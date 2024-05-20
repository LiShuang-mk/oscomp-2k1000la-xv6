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
		int register_device(
			uint dev_num,
			DeviceOperator dev_read,
			DeviceOperator dev_write
		);

	};

	extern DeviceManager k_dm;

} // namespace dev
