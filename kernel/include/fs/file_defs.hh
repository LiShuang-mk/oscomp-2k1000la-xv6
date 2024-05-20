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
			int r : 1;    //read
			int w : 1;    //write
			int a : 1;    //append
		} fields;   //file's  authority
		FileOp rwa;
		FileOps( FileOp op_ = FileOp::fileop_none ) : rwa( op_ ) {}
		FileOps( int flags_ )
		{
			fields.r = flags_ & FA_READ;
			fields.w = flags_ & FA_WRITE;
			fields.a = flags_ & FA_APPEND;
		}
	};
	
} // namespace fs
