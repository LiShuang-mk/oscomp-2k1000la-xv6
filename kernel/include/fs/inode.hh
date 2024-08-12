//
// Created by Li Shuang ( pseudonym ) on 2024-05-19
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#pragma once

#include <EASTL/string.h>

#include "fs/file/file_defs.hh"
#include "types.hh"

namespace mm
{
	class UserspaceStream;
}

namespace fs
{
	class DStat;
	class SuperBlock;
	class FileSystem;

	class Inode
	{
	public:

		Inode()						= default;
		Inode( const Inode& inode ) = default;
		virtual ~Inode()			= default;
		Inode& operator=( const Inode& inode );

		// virtual void link(const char *name,InodeRef nod) = 0;
		// virtual int nodeHardUnlink() = 0;
		virtual Inode* lookup( eastl::string dirname )		 = 0;
		virtual Inode* mknode( eastl::string name, FileAttrs attr,
							   eastl::string dev_name = "" ) = 0;
		// virtual int entSymLink( const eastl::string arget ) = 0;

		// virtual void nodeRemove() = 0;
		// virtual int chMod( uint32 mode ) = 0;
		// virtual int chOwn(uid_t uid, gid_t gid) = 0; //change group owner
		// virtual void nodeTrunc() = 0;

		virtual size_t nodeRead( uint64 dst_, size_t off_, size_t len_ )  = 0;
		virtual size_t nodeWrite( uint64 src_, size_t off_, size_t len_ ) = 0;

		using ubuf = mm::UserspaceStream;
		struct linux_dirent64
		{
			u64	 d_ino;	   /* 64-bit inode number */
			u64	 d_off;	   /* Not an offset; see getdents() */
			u16	 d_reclen; /* Size of this dirent */
			u8	 d_type;   /* File type */
			char d_name[]; /* Filename (null-terminated) */
		} __attribute__( ( packed ) );
		virtual size_t readSubDir( ubuf& dst, size_t off ) = 0;

		// void readvec();
		// void readPages();

		// virtual int readLink( char *buf, uint64 len ) = 0;
		virtual int readlinkat( char* buf, uint64 len ) = 0;
		// virtual int readDir( DStat *buf, uint32 len, uint64 off_ ) = 0;
		// virtual int ioctl( uint64 req, uint64 arg ) = 0;

		virtual FileAttrs rMode() const		= 0; // get mode
		virtual dev_t	  rDev() const		= 0; // get device number
		virtual uint64	  rFileSize() const = 0; // get file size
		virtual uint64	  rIno() const		= 0; // get inode number

		// Ctime, Mtime, Atime temporarily not implemented
		// virtual bool Empty() const = 0; // check if inode is empty
		virtual SuperBlock* getSb() const = 0; // get super block
		virtual FileSystem* getFS() const = 0; // get file system
		// virtual int create( Inode *dir, Dentry *dentry, eastl::string name,
		// uint32 mode ) = 0; virtual int mkdir( Inode *dir, Dentry *dentry,
		// eastl::string name, uint32 mode ) = 0;
	};

} // namespace fs
