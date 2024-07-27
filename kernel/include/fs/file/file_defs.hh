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

/*
	POSIX 文件权限和类型定义
*/
#define S_IRUSR 0x0100 // 用户读权限
#define S_IWUSR 0x0080 // 用户写权限
#define S_IXUSR 0x0040 // 用户执行权限
#define S_IRGRP 0x0020 // 组读权限
#define S_IWGRP 0x0010 // 组写权限
#define S_IXGRP 0x0008 // 组执行权限
#define S_IROTH 0x0004 // 其他人读权限
#define S_IWOTH 0x0002 // 其他人写权限
#define S_IXOTH 0x0001 // 其他人执行权限

	struct FileAttrs
	{
		FileTypes filetype;
		uint u_read : 1;  // 用户读权限
		uint u_write : 1; // 用户写权限
		uint u_exec : 1;  // 用户执行权限
		uint g_read : 1;  // 组读权限
		uint g_write : 1; // 组写权限
		uint g_exec : 1;  // 组执行权限
		uint o_read : 1;  // 其他读权限
		uint o_write : 1; // 其他写权限
		uint o_exec : 1;  // 其他执行权限
		// union
		// {
		// 	u16 _value;
		// 	struct
		// 	{
		// 		u16 u_read : 1;  // 用户读权限
		// 		u16 u_write : 1; // 用户写权限
		// 		u16 u_exec : 1;  // 用户执行权限
		// 		u16 g_read : 1;  // 组读权限
		// 		u16 g_write : 1; // 组写权限
		// 		u16 g_exec : 1;  // 组执行权限
		// 		u16 o_read : 1;  // 其他读权限
		// 		u16 o_write : 1; // 其他写权限
		// 		u16 o_exec : 1;  // 其他执行权限
		// 		u16 _rsv : 7;
		// 	}__attribute__((__packed__));
		// }__attribute__((__packed__));

		FileAttrs(mode_t mode_ = 0) : filetype(FT_NONE)
		{
			u_read = (mode_ & S_IRUSR) ? 1 : 0;
			u_write = (mode_ & S_IWUSR) ? 1 : 0;
			u_exec = (mode_ & S_IXUSR) ? 1 : 0;
			g_read = (mode_ & S_IRGRP) ? 1 : 0;
			g_write = (mode_ & S_IWGRP) ? 1 : 0;
			g_exec = (mode_ & S_IXGRP) ? 1 : 0;
			o_read = (mode_ & S_IROTH) ? 1 : 0;
			o_write = (mode_ & S_IWOTH) ? 1 : 0;
			o_exec = (mode_ & S_IXOTH) ? 1 : 0;
		}

		FileAttrs(FileTypes type_, mode_t mode_) : filetype(type_)
		{
			u_read = (mode_ & S_IRUSR) ? 1 : 0;
			u_write = (mode_ & S_IWUSR) ? 1 : 0;
			u_exec = (mode_ & S_IXUSR) ? 1 : 0;
			g_read = (mode_ & S_IRGRP) ? 1 : 0;
			g_write = (mode_ & S_IWGRP) ? 1 : 0;
			g_exec = (mode_ & S_IXGRP) ? 1 : 0;
			o_read = (mode_ & S_IROTH) ? 1 : 0;
			o_write = (mode_ & S_IWOTH) ? 1 : 0;
			o_exec = (mode_ & S_IXOTH) ? 1 : 0;
		}

		mode_t transMode()
		{
			mode_t mode = 0;
			if (u_read)
				mode |= S_IRUSR;
			if (u_write)
				mode |= S_IWUSR;
			if (u_exec)
				mode |= S_IXUSR;
			if (g_read)
				mode |= S_IRGRP;
			if (g_write)
				mode |= S_IWGRP;
			if (g_exec)
				mode |= S_IXGRP;
			if (o_read)
				mode |= S_IROTH;
			if (o_write)
				mode |= S_IWOTH;
			if (o_exec)
				mode |= S_IXOTH;
			return mode;
		}
	};

} // namespace fs
