//
// Created by Li Shuang ( pseudonym ) on 2024-05-19 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

namespace fs
{
	enum FileTypes
	{
		FT_NONE,
		FT_PIPE,
		FT_STDIN,
		FT_STDOUT,
		FT_STDERR,
		FT_INODE,
		FT_DEVICE,
		FT_ENTRY
	};

	enum FileOp : uint16
	{
		fileop_none = 0x00,
		fileop_read = 0x01,
		fileop_write = 0x02,
		fileop_append = 0x04,
	};

	enum FileAuthority
	{
		FA_READ = 0x01,
		FA_WRITE = 0x02,
		FA_APPEND = 0x04,
	};

	union FileOps
	{
		struct
		{
			uint r : 1;    //read
			uint w : 1;    //write
			uint a : 1;    //append
		} fields;   //file's  authority
		FileOp rwa;
		FileOps( FileOp op_ = FileOp::fileop_none ) : rwa( op_ ) {}
		FileOps( int flags_ )
		{
			fields.r = ( flags_ & FA_READ ) ? 1 : 0;
			fields.w = ( flags_ & FA_WRITE ) ? 1 : 0;
			fields.a = ( flags_ & FA_APPEND ) ? 1 : 0;
		}
	};

	/*
		1. 	POSIX 文件权限和类型定义
	*/
	enum FileAttrs
	{
		File_dir = 0x01,

		File_u_read = 0x04,
		File_u_write = 0x02,
		File_u_exec = 0x01,

		File_g_read = 0x04,
		File_g_write = 0x02,
		File_g_exec = 0x01,

		File_o_read = 0x04,
		File_o_write = 0x02,
		File_o_exec = 0x01,

		File_dir_s = 15, // 最高位[15]为1表示是目录,否则是一个普通文件
		File_u_s = 12,	// 用户权限位
		File_g_s = 9,	// 组权限位
		File_o_s = 6,	// 其他权限位

	};

#define ISDIR( m ) ((m) & ( FileAttrs::File_dir << FileAttrs::File_dir_s))


} // namespace fs
