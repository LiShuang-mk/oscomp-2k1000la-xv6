#include "fs/fat/fat32fs.hh"
#include "fs/fat/fat32Dentry.hh"
#include "fs/dentry.hh"
#include "fs/buffer_manager.hh"
#include "fs/buffer.hh"

namespace fs{
    namespace fat{

        Fat32FS k_fatfs;
        
        void Fat32FS::init(){
            /// @todo 1. load super block
            Buffer buf = k_bufm.read_sync( 1, 0 );
			Fat32Dbr dbr = *( Fat32Dbr* ) buf.get_data_ptr();

            _super_block = new Fat32SuperBlock( this, &dbr, true );

            /// @todo 2. set _start_lba, _fat_lba, _device
            _device = 1;
            _start_lba = 0;
            _fat_lba = _start_lba + _super_block->get_bpb()->reserved_sector_count;
            _isroot = true;

            // @todo 3. init rootEntry, and it's fat32inode.
            _root = new Fat32Dentry();
            _root->init( 1 , _super_block, this);
            _mnt = _root;


        }
    }
}