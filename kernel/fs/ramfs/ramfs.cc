#include "fs/ramfs/ramfs.hh"
#include "fs/ramfs/ramfsInode.hh"
#include "fs/dentrycache.hh"
#include "fs/dentry.hh"
#include "fs/file/file.hh"
#include "fs/fat/fat32fs.hh"
#include "fs/ext4/ext4_fs.hh"
#include "fs/path.hh"

#include "device_manager.hh"
#include "hsai_global.hh"
#include <hsai_defs.h>
#include "EASTL/queue.h"

using namespace fs::dentrycache;

namespace fs
{

	namespace ramfs
	{

		RamFS k_ramfs;

		dentry *RamFS::getRoot() const
		{
			return _root;
		}

		void RamFS::initfd()
		{

/// set device num
			_device = 0; /// 这里设备号没什么用，ramfs是一个内存文件系统
			/// set file system type
			_fstype = "ramfs";
			/// super block has been initialized in constructor
			_super_block = new RamFSSb( this );
			// _root = new fs::dentry( "/", _super_block->allocInode(
			//                     FileAttrs::File_dir << FileAttrs::File_dir_s )
			//                     , nullptr, true );
			FileAttrs attrs = _super_block->rDefaultMod();
			attrs.filetype = FileTypes::FT_DIRECT;
			_root = fs::dentrycache::k_dentryCache.alloDentry();
			new ( _root ) dentry( "/", _super_block->allocInode(
				attrs )
				, nullptr, true );
			_isroot = true;
			_mnt = nullptr;

			_root->EntryCreate( "dev", attrs ); // 暂时把第二个参数为正数认为是目录， 还要创建 /dev/sda1, 用来初始化ext4
			_root->EntryCreate( "proc", attrs );
			_root->EntryCreate( "sys", attrs );
			_root->EntryCreate( "tmp", attrs );
			_root->EntryCreate( "mnt", attrs );
			_root->EntryCreate( "bin", attrs );

			dentry * etc_dent = _root->EntryCreate( "etc", attrs );
			dentry * conf_dent = etc_dent->EntryCreate( "busybox.conf", attrs );
			( ( RamInode * ) conf_dent->getNode() )->readable = true;

			_root->printChildrenInfo();

			// init fat
			dentry* dev = _root->EntrySearch( "dev" );
			if ( !dev )
			{
				log_error( "RamFS::initfd: mnt is nullptr" );
				return;
			}

			char ** dev_table = new char*[ DEV_TBL_LEN ];

			for ( int i = 0; i < DEV_TBL_LEN; i++ )
			{
				dev_table[ i ] = nullptr;
			}
			hsai::k_devm.traversal_dev_table( dev_table );
			for ( int i = 0; i < DEV_TBL_LEN; i++ )
			{
				if ( dev_table[ i ] == nullptr )
					break;
				dev->EntryCreate( dev_table[ i ], _super_block->rDefaultMod(), dev_table[ i ] );
			}
			
			dentry *rtc = dev->EntryCreate( "rtc", FileAttrs( FileTypes::FT_DEVICE, 0444 ) );
			RTC *rtc_ = new RTC( static_cast<RamFS*>(rtc->getNode()->getFS()), 9, 100);
			rtc->setNode( rtc_ );

			// init /proc
			dentry *proc = _root->EntrySearch( "proc" );
			if (!proc )
			{
				log_error( "RamFS::initfd: proc is nullptr" );
                return;
			}
			
			proc->EntryCreate( "self", FileAttrs( FileTypes::FT_DIRECT, 0444) );
			
			// init /proc/meminfo
			dentry *meminfo = proc->EntryCreate( "meminfo", FileAttrs( FileTypes::FT_NORMAL, 0444 ) );
			MemInfo *meminfo_ = new MemInfo( static_cast<RamFS*>(meminfo->getNode()->getFS()), 10 );
			meminfo->setNode( meminfo_ );


			dentry *self = proc->EntrySearch( "self" );
			// init exe
			dentry *exe = self->EntryCreate( "exe", FileAttrs( FileTypes::FT_NORMAL, 0444 ) );
			Exe *exe_ = new Exe( static_cast<RamFS*>(self->getNode()->getFS()), 11 );
			exe->setNode( exe_ );


			//init mount
			dentry *mounts = proc->EntryCreate( "mounts", FileAttrs( FileTypes::FT_DIRECT, 0444 ) );
			Mount *mnt_ = new Mount( static_cast<RamFS*>(mounts->getNode()->getFS()), 12, FileAttrs( FileTypes::FT_SYMLINK, 0444 ) );
			mounts->setNode( mnt_ );


			// init bin
			dentry *bin = _root->EntrySearch( "bin" );
            if (!bin )
            {
                log_error( "RamFS::initfd: bin is nullptr" );
                return;
            }

            dentry *ls = bin->EntryCreate( "ls", attrs ); // 创建ls
			SymbleLink *ls_link_ = new SymbleLink( static_cast<RamFS*>(mounts->getNode()->getFS()), 
													13,
													FileAttrs( FileTypes::FT_NORMAL, 0777 ),  // 这里应该是一个SYMBLE_LINK
                                                    "/mnt/sdcard/busybox" );
			ls->setNode( ls_link_ );


			_root->printChildrenInfo();
			return;
		}

		RamFS::RamFS()
		{
			_super_block = new RamFSSb( this );
		}


		// 关于mount， 目前只考虑挂载FS的情况，提供参数有  special, mnt, fstype, flags, data，
		// 首先对给定的 special 和 mnt的路径进行解析，拿到对应的dentry， 然后进行挂载，具体为
		// 对special 对应设备中的FS初始化，然后把他的 mnt指向 当前的挂载点供 卸载文件系统的时候恢复原dentry
		// 然后替换dentry 即可。 目前的flags 和 data先不进行考虑

		int RamFS::mount( dentry *dev, dentry *mount, eastl::string fstype )
		{
			if ( mount == nullptr || dev == nullptr )
			{
				log_error( "RamFS::mount: mount or dev is nullptr" );
				return -1;
			}

			if ( mount->isRoot() )
			{
				log_error( "RamFS::mount: mount is root or mount point" );
				return -1;
			}

			if ( mount->getNode()->rMode().filetype != FileTypes::FT_DIRECT )
			{
				log_error( "RamFS::mount: mount is not a directory" );
				return -1;
			}

			// 具体fs挂载初始化，先写个Fat32
			if ( fstype == "fat32" || fstype == "vfat" )
			{
				// fs'_mnt 保存原挂载点信息
				fs::fat::Fat32FS *fatfs = new fs::fat::Fat32FS( false, mount );
				//fatfs->init( dev->getNode()->rDev(), 0, fstype );
				fatfs->init( dev->getNode()->rDev(), 0, fstype, mount->rName() );

				dentry *root = fatfs->getRoot();
				[[maybe_unused]] dentry *parent = mount->getParent();

				dentry *mnt = mount;
				eastl::string mnt_path;
				while( mnt != nullptr )
				{
					mnt_path = mnt->rName() + "/" + mnt_path;
					mnt = mnt->getParent();
				}
				mnt_path = mnt_path.substr( 1, mnt_path.size() - 1 );

				root->setParent( mount->getParent() );
				mount->getParent()->getChildren()[ mount->rName() ] = fatfs->getRoot();
				for ( auto it : mount->getChildren() )
				{
					log_info( "RamFS::mount: %s", it.second->rName().c_str() );
				}

				fs::mnt_table[ mnt_path ] = fatfs;
			}
			else if ( fstype == "ext4" )
			{
				fs::ext4::Ext4FS * ext4fs = new fs::ext4::Ext4FS();
				ext4fs->init( dev->getNode()->rDev(), 0, fstype, mount->rName() );

				dentry *root = ext4fs->getRoot();
				[[maybe_unused]] dentry *parent = mount->getParent();

				dentry *mnt = mount;
				eastl::string mnt_path;
				while( mnt != nullptr )
				{
					mnt_path = mnt->rName() + "/" + mnt_path;
					mnt = mnt->getParent();
				}

				mnt_path = mnt_path.substr( 1, mnt_path.size() - 2 );

				root->setParent( mount->getParent() );
				mount->getParent()->getChildren()[ mount->rName() ] = ext4fs->getRoot();
				for ( auto it : mount->getChildren() )
				{
					log_info( "RamFS::mount: %s", it.second->rName().c_str() );
				}

				fs::mnt_table[ mnt_path ] = ext4fs;
			}
			else
			{
				log_error( "RamFS::mount: unknown file system type" );
				return -1;
			}
			_root->printChildrenInfo();
			return 0;
		}

		int RamFS::umount( dentry *mount )
		{
			if ( mount == nullptr )
			{
				log_error( "RamFS::umount: mount is nullptr" );
			}

			if ( !mount->isRoot() )
			{
				log_error( "RamFS::umount: mount is not mount point" );
				return -1;
			}
			dentry *parent = mount->getParent();
			dentry *mnt = mount->getNode()->getFS()->getMntPoint();
			parent->getChildren()[ mount->rName() ] = mnt;
			_root->printChildrenInfo(); // 
			return 0; //卸载完成
		}

	}
}