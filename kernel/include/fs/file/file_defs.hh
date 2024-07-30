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

	#define AT_FDCWD		-100 //current dir
	enum FileTypes
	{
		FT_NONE,
		FT_PIPE,
		FT_DEVICE,
		FT_DIRECT,
		FT_NORMAL
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
			uint r : 1; // read
			uint w : 1; // write
			uint a : 1; // append
		} fields;		// file's  authority
		FileOp rwa;
		FileOps(FileOp op_ = FileOp::fileop_none) : rwa(op_) {}
		FileOps(int flags_)
		{
			fields.r = (flags_ & FA_READ) ? 1 : 0;
			fields.w = (flags_ & FA_WRITE) ? 1 : 0;
			fields.a = (flags_ & FA_APPEND) ? 1 : 0;
		}
	};



	struct FileAttrs
	{
		FileTypes filetype;
		union
		{
			u16 _value;
			struct
			{
				u16 o_exec : 1;  // 其他执行权限
				u16 o_write : 1; // 其他写权限
				u16 o_read : 1;  // 其他读权限
				u16 g_exec : 1;  // 组执行权限
				u16 g_write : 1; // 组写权限
				u16 g_read : 1;  // 组读权限
				u16 u_exec : 1;  // 用户执行权限
				u16 u_write : 1; // 用户写权限
				u16 u_read : 1;  // 用户读权限
				u16 _rsv : 7;
			}__attribute__((__packed__));
		}__attribute__((__packed__));

		FileAttrs(mode_t mode_ = 0) : filetype(FT_NONE), _value( mode_ ){}

		FileAttrs(FileTypes type_, mode_t mode_) : filetype(type_), _value( mode_){}

		mode_t transMode()
		{
			return _value & 0x01FF;
		}
	};
}