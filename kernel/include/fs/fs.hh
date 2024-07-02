#pragma once
#include "types.hh"
#include "smp/lock.hh"
#include "fs/fs_defs.hh"
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/unordered_map.h>

#define NDIRECT 12  //direct block number

namespace fs
{
	class FileSystem;
	class Dentry;
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
		virtual void init( int dev, uint64 start_lba, bool is_root ) = 0;
		//virtual eastl::string rFSType() const = 0; // get file system type
		//virtual eastl::string rKey() const = 0; // get fs's key in mount map
		virtual bool isRootFS() const = 0; // check if it is root fs
		virtual SuperBlock *getSuperBlock() const = 0; // get super block
		//virtual int ldSuperBlock( uint64 dev, Dentry * mnt ) = 0; // load super block
		//virtual void unInstall() = 0; // uninstall file system
		//virtual long rMagic() const = 0; // get magic number
		virtual size_t rBlockSize() const = 0; // get block size
		virtual long rBlockNum() const = 0; // get block number
		virtual long rBlockFree() const = 0; // get free block number
		virtual long rMaxFile() const = 0; // max file num when read
		virtual long rFreeFile() const = 0; // max file num when read free block
		virtual eastl::string rFStype() const = 0; // get filesystem type
		virtual size_t rNamelen() const = 0; // get max name length
		virtual Dentry *getRoot() const = 0; // get root dentry
		virtual Dentry *getMntPoint() const = 0; // get mount point dentry
	};

	// Dentry * DentryRef;

}
