#pragma once
#include "types.hh"
#include "fs/fs_defs.hh"
#include "fs/file/file_defs.hh"

#include <smp/spin_lock.hh>

#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/unordered_map.h>

//#include <asm-generic/statfs.h>

#define NDIRECT 12  //direct block number

namespace fs
{
	class FileSystem;
	class Inode;
	// class File;
	// class DStat;
	// class ProcessorManager;

	class FileSystem
	{
	public:
		FileSystem() = default;
		FileSystem( const FileSystem& fs ) = delete;
		virtual ~FileSystem() = default;
		FileSystem& operator=( const FileSystem& fs );
		virtual void init( int dev, uint64 start_lba, eastl::string fstype,eastl::string rootname, bool is_root ) = 0;
		//virtual eastl::string rFSType() const = 0; // get file system type
		//virtual eastl::string rKey() const = 0; // get fs's key in mount map
		virtual bool isRootFS() const = 0; // check if it is root fs
		virtual SuperBlock *getSuperBlock() const = 0; // get super block
		//virtual int ldSuperBlock( uint64 dev, Dentry * mnt ) = 0; // load super block
		//virtual void unInstall() = 0; // uninstall file system
		virtual long rMagic() const = 0; // get magic number
		virtual size_t rBlockSize() const = 0; // get block size
		virtual long rBlockNum() const = 0; // get block number
		virtual long rBlockFree() const = 0; // get free block number
		virtual long rMaxFile() const = 0; // max file num when read
		virtual long rFreeFile() const = 0; // max file num when read free block
		virtual eastl::string rFStype() const = 0; // get filesystem type
		virtual size_t rNamelen() const = 0; // get max name length
		virtual dentry *getRoot() const = 0; // get root dentry
		virtual dentry *getMntPoint() const = 0; // get mount point dentry
		virtual FileAttrs rDefaultMod() const = 0;
		virtual int mount( dentry *dev, dentry *mnt, eastl::string fstype ) = 0 ;
		virtual int umount( dentry *mnt ) = 0;

	};


	class statfs
	{
		private:
        	uint64 f_bsize;  		// 块大小
			uint64 f_frsize;		// 基本文件系统块大小，大部分和bsize一致
			uint64 f_blocks;		// 文件系统数据块总数
			uint64 f_bfree;			// 可用块数
			uint64 f_bavail;		// 非超级用户可用块数
			uint64 f_files;			// 文件结点总数
			uint64 f_ffree;			// 可用文件结点数
			uint64 f_favail;		// 非超级用户可用文件结点数
			uint64 f_fsid;			// 文件系统标识
			uint64 f_flag;			// 挂载标志
			uint64 f_namemax;		// 最大文件名长度
			int __f_spare[6];		// 保留
		
		public:
			statfs() = default;
			statfs( const statfs& ) = default;
			statfs( const FileSystem& fs ) 	: 	f_bsize( fs.rBlockSize() ),
												f_frsize( fs.rBlockSize() ),
												f_blocks( fs.rBlockNum() ),
												f_bfree( fs.rBlockFree() ),
												f_bavail( fs.rBlockFree() ),
												f_files( fs.rMaxFile() ),
												f_ffree( fs.rFreeFile() ),
												f_favail( fs.rFreeFile() ),
												f_fsid( fs.rMagic() ),
												f_flag( 0 ),
												f_namemax( fs.rNamelen() )
												{}
			~statfs() = default;

	};

}
