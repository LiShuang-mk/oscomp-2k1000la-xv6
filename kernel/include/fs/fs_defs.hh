//
// Created by Li Shuang ( pseudonym ) on 2024-05-19 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "klib/common.hh"
#include "fs/file/file_defs.hh"

#include <EASTL/string.h>
namespace fs
{
	class FileSystem;
	class Inode;
	class dentry;
	class SuperBlock
	{
	public:
		SuperBlock() = default;
		SuperBlock( const SuperBlock& sb ) = delete;
		virtual ~SuperBlock() = default;
		SuperBlock & operator=( const SuperBlock& sb );
		virtual dentry* getRoot() const = 0;     // get root dentry  
		virtual dentry* getMntPoint() const = 0; // get mount point dentry
		virtual FileSystem* getFileSystem() const = 0; // get file system
		virtual bool isValid() const = 0; // check if super block is valid
		virtual FileAttrs rDefaultMod() const = 0; // get default mode
		virtual size_t rBlockSize() const = 0; // get block size
		virtual size_t rBlockNum() const = 0; // get block number
		virtual Inode *allocInode( FileAttrs attrs , eastl::string dev_name = "") = 0; // allocate inode
		//virtual void destroyInode( Inode *inode ) = 0; // destroy inode
	};


} // namespace fs
