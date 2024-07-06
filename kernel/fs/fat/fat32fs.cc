#include "fs/fat/fat32fs.hh"
//#include "fs/fat/fat32Dentry.hh"
#include "fs/dentry.hh"
#include "fs/buffer_manager.hh"
#include "fs/buffer.hh"
#include "fs/dentrycache.hh"

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

		void Fat32FS::init( int dev, uint64 start_lba, bool is_root )
		{
			/// @todo 1. load super block
			Buffer buf = k_bufm.read_sync( dev, start_lba );
			Fat32Dbr *dbr = ( Fat32Dbr* ) buf.get_data_ptr();
			assert( dbr->bootable_signature == 0xAA55, "Fat32FS : incorrect DBR loaded." );

			_super_block = new Fat32SuperBlock( this, dbr, true );

			/// @todo 2. set _start_lba, _fat_lba, _device
			_fstype = "fat32";
			_device = dev;
			_start_lba = start_lba;
			_fat_lba = _start_lba + _super_block->get_bpb()->reserved_sector_count;
			_isroot = is_root;

			/// @todo 3. init rootEntry, and it's fat32inode.
			_root = new Fat32Dentry();
			_root->init( _device, _super_block, this );
			_mnt = _root;

			_root->EntryCreate( "test_unlink", 0 );
		}

	}
}