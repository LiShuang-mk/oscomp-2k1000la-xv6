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
		//virtual Inode *allocInode() = 0; // allocate inode
		//virtual void destroyInode( Inode *inode ) = 0; // destroy inode
	};
} // namespace fs
