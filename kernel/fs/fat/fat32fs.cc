#include "fs/fat/fat32fs.hh"
#include "fs/fat/fat32inode.hh"
#include "fs/dentry.hh"
#include "fs/buffer_manager.hh"
#include "fs/buffer.hh"
#include "fs/dentrycache.hh"

#include "fs/path.hh"

namespace fs
{
	namespace fat
	{

		Fat32FS k_fatfs;

		Fat32FS::~Fat32FS()
		{
			delete _super_block;
			delete _root;
		}

		void Fat32FS::init( int dev, uint64 start_lba, eastl::string fstype, eastl::string rootname, bool is_root )
		{
			/// @todo 1. load super block
			Buffer buf = k_bufm.read_sync( dev, start_lba );
			Fat32Dbr *dbr = ( Fat32Dbr* ) buf.get_data_ptr();
			assert( dbr->bootable_signature == 0xAA55, "Fat32FS : incorrect DBR loaded." );

			_super_block = new Fat32SuperBlock( this, dbr, true );

			/// @todo 2. set _start_lba, _fat_lba, _device
			_fstype = fstype;
			_device = dev;
			_start_lba = start_lba;
			_fat_lba = _start_lba + _super_block->get_bpb()->reserved_sector_count;
			_isroot = is_root;

			// /// @todo 3. init rootEntry, and it's fat32inode.

			Fat32Inode *_node = static_cast<Fat32Inode *>( _super_block->allocInode( _super_block->rDefaultMod() ) );
			_node->init( 2, this, fat32nod_folder, _super_block->rDefaultMod() );  // init as 777 
			//_root = new dentry(rootname, _node, nullptr, false );
			_root = fs::dentrycache::k_dentryCache.alloDentry();
			new ( _root ) dentry( rootname, _node, nullptr, false );
			//_mnt = nullptr;

			_root->EntryCreate( "test_unlink", 0 );

			/// @todo 4. try to mount fat to ramfs's /mnt/fat
		}

		dentry *Fat32FS::getRoot () const
		{
			return _root;
		}

		int Fat32FS::umount ( dentry *mount )
		{
			return 0;
		}

	}
}