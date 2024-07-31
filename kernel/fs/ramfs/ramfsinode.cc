#include "fs/ramfs/ramfsInode.hh"
#include "fs/ramfs/ramfs.hh"

#include "hsai_global.hh"
#include "device_manager.hh"

namespace fs
{

	namespace ramfs
	{


		SuperBlock *RamInode::getSb() const
		{
			return belong_fs->getSuperBlock();
		}

		FileSystem *RamInode::getFS() const
		{
			return belong_fs;
		}

		Inode *RamInode::mknode( eastl::string name, FileAttrs attrs, eastl::string dev_name )
		{
			return belong_fs->getSuperBlock()->allocInode( attrs, dev_name );
		}

		dev_t RamInode::rDev() const
		{
			if ( dev_name.empty() )
				log_error( "This is not a valid device file" );
			return hsai::k_devm.search_device( dev_name.c_str() );
		}

		static char busybox_conf[] =
			"# /etc/busybox.conf: SliTaz GNU/linux Busybox configuration.\n"
			"[SUID]\n"
			"# Allow command to be run by anyone.\n"
			"su = ssx root.root\n"
			"passwd = ssx root.root"
			"loadkmap = ssx root.root\n"
			"mount = ssx root.root\n"
			"reboot = ssx root.root\n"
			"halt = ssx root.root\n"
			"poweroff = ssx root.root\n";

		size_t RamInode::nodeRead( uint64 dst_, size_t off_, size_t len_ )
		{
			if ( readable )
			{
				size_t read_len = ( off_ + len_ > sizeof busybox_conf ) ? ( ( sizeof busybox_conf ) - off_ ) : len_;
				memcpy( ( void * ) dst_, ( void * ) &busybox_conf[ off_ ], read_len );
				return read_len;
			}
			else return 0;
		}
	}
}