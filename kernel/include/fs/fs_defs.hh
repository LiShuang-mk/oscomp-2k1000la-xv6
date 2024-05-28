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
	class Dentry;
	class FileSystem;
	class Inode;

	class SuperBlock
	{
	public:
		SuperBlock() = default;
		SuperBlock( const SuperBlock& sb ) = delete;
		virtual ~SuperBlock() = default;
		SuperBlock & operator=( const SuperBlock& sb );
		virtual Dentry* getRoot() const = 0;     // get root dentry  
		virtual Dentry* getMntPoint() const = 0; // get mount point dentry
		virtual FileSystem* getFileSystem() const = 0; // get file system
		virtual bool isValid() const = 0; // check if super block is valid
		virtual uint32 rDefaultMod() const = 0; // get default mode
		virtual size_t rBlockSize() const = 0; // get block size
		virtual size_t rBlockNum() const = 0; // get block number
		//virtual Inode *allocInode() = 0; // allocate inode
		//virtual void destroyInode( Inode *inode ) = 0; // destroy inode
	};


	/*	
		1. 	POSIX 文件权限和类型定义
	*/
	enum FileAttrs{
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
} // namespace fs
