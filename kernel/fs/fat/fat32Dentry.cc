#include "fs/fat/fat32Dentry.hh"
#include "fs/fat/fat32Sb.hh"
#include "fs/buffer.hh"
#include "fs/buffer_manager.hh"
#include "fs/fat/fat32Sb.hh"
#include "fs/fat/fat32fs.hh"

#include <EASTL/vector.h>

namespace fs{
    namespace fat{
        void Fat32Dentry::init(uint32 dev, Fat32SuperBlock *sb, Fat32FS *fs){

            uint cluster_num = 2;
            uint64 cls_num_in_sector = cluster_num / 
                    (fs->get_bytes_per_sector() / sizeof(cluster_num_t));
            uint64 cls_num_offset = cluster_num % 
                    (fs->get_bytes_per_sector() / sizeof(cluster_num_t));

            Buffer buf =  k_bufm.read_sync(
                dev,
                fs->get_fat_lba() + cls_num_in_sector
            );
            
            eastl::vector<uint32> clusters_num;
            clusters_num.clear();

            using fat32_t = uint32;
            fat32_t * fat32_p = ( fat32_t * ) buf.get_data_ptr();
            fat32_p += cls_num_offset;
            fat32_t * buf_end = ( fat32_t * ) buf.get_end_ptr();
            for( fat32_t i = cluster_num; fat32_p < buf_end; fat32_p++ )
            {
                clusters_num.push_back( i );
                if ( fat_is_end( *fat32_p) )
                    break;
                i = *fat32_p;
            }
            k_bufm.release_buffer_sync( buf );

            assert(
				fat32_p < buf_end,
				"Fat32DirEntry: too long directory that covers more than %d clusters.",
				default_buffer_size / sizeof( uint32 )
			);
            
            _node = new Fat32Inode();
            _node->init( dev, 
                         clusters_num.front(),
                         fs, 
                         fat32nod_folder, 
                         clusters_num, 
                         0 );
                    
            rootInit( _node );
        }

        Dentry * Fat32Dentry::EntrySearch(eastl::string name){
            log_trace( "Fat32Dentry::EntrySearch: name = %s", name.c_str() );
            if( auto it = _dentry_children.find( name ); it != _dentry_children.end() )
                return it->second;
            if(auto subnod = dynamic_cast<Fat32Inode*>( _node->lookup( name )  ) ){
                log_trace( "Fat32Dentry::EntrySearch: found inode" );
                auto subdentry = new Fat32Dentry(this, subnod, name);
                _dentry_children[name] = subdentry;
                return subdentry;
            }
            return nullptr;
        
        }
    }
}