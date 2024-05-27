#include "fs/fat/fat32inode.hh"
#include "fs/fat/fat32fs.hh"
#include "fs/buffer.hh"
#include "fs/buffer_manager.hh"
#include "fs/inode.hh"

#include <EASTL/vector.h>

namespace fs{

    namespace fat{
        
        void Fat32Inode::read_content( void * buf, uint64 read_len, uint64 offset, bool async_read )
		{
			if ( _dir_type == fat32nod_folder )
				log_warn( "Fat32DirEntry: read folder entry content will get raw data." );

			uint64 len;
			uint64 cover_size = _cover_size_bytes();
			len = ( cover_size - offset > read_len ) ? read_len : cover_size;

			uint64 cls_start = offset /
				( _belong_fs->get_sectors_per_cluster() * _belong_fs->get_bytes_per_sector() );
			if ( cls_start >= _clusters_number.size() )
			{
				log_warn( "read offset beyond file size. no data will be read." );
				return;
			}

			Buffer disk_buf;
			uint64 rest_size = len;
			uint64 writ_head = ( uint64 ) buf;
			offset %= _belong_fs->get_bytes_per_sector() * _belong_fs->get_sectors_per_cluster();

			// read first cluster
			if ( async_read ) disk_buf = k_bufm.read( _belong_fs->owned_device(), _cluster_to_lba( _clusters_number[ cls_start ] ) );
			else disk_buf = k_bufm.read_sync( _belong_fs->owned_device(), _cluster_to_lba( _clusters_number[ cls_start ] ) );

			if ( rest_size > default_buffer_size )
			{
				memcpy( ( void * ) writ_head, ( void* ) ( ( uint64 ) disk_buf.get_data_ptr() + offset ), default_buffer_size );
				rest_size -= default_buffer_size;
				writ_head += default_buffer_size;
			}
			else
			{
				memcpy( ( void * ) writ_head, ( void* ) ( ( uint64 ) disk_buf.get_data_ptr() + offset ), rest_size );
				rest_size = 0;
				writ_head += rest_size;
			}

			if ( async_read ) k_bufm.release_buffer( disk_buf );
			else k_bufm.release_buffer_sync( disk_buf );

			// read rest cluster
			cls_start++;
			for ( uint64 i = cls_start; i < _clusters_number.size(); ++i )
			{
				if ( rest_size == 0 )
					break;

				if ( async_read ) disk_buf = k_bufm.read( _belong_fs->owned_device(), _cluster_to_lba( _clusters_number[ i ] ) );
				else disk_buf = k_bufm.read_sync( _belong_fs->owned_device(), _cluster_to_lba( _clusters_number[ i ] ) );

				if ( rest_size > default_buffer_size )
				{
					memcpy( ( void * ) writ_head, disk_buf.get_data_ptr(), ( int ) default_buffer_size );
					rest_size -= default_buffer_size;
					writ_head += default_buffer_size;
				}
				else
				{
					memcpy( ( void * ) writ_head, disk_buf.get_data_ptr(), rest_size );
					rest_size = 0;
					writ_head += rest_size;
				}

				if ( async_read ) k_bufm.release_buffer( disk_buf );
				else k_bufm.release_buffer_sync( disk_buf );
			}
		}
   
        uint64 Fat32Inode::_cluster_to_lba( uint64 cluster )
		{
			return
				_belong_fs->start_lba() +	// fat32 start lba
				_belong_fs->get_reserved_sectors() +	// skip reserved sectors, including BPB
				_belong_fs->get_sectors_per_fat() * _belong_fs->get_fat_counts() +	// skip File Allocation Table
				_belong_fs->get_sectors_per_cluster() * ( cluster - Fat32_first_data_cluster );	// sector offset in DATA
		}

        uint64 Fat32Inode::_cover_size_bytes()
		{
			return
				_clusters_number.size()
				* _belong_fs->get_sectors_per_cluster()
				* _belong_fs->get_bytes_per_sector();
		}

		SuperBlock * Fat32Inode::getSb() const {
			return _belong_fs->getSuperBlock();
		}
	
		Inode * Fat32Inode::lookup( eastl::string dirname, off_t off_ )
		{
			if( !is_dir() ) return nullptr;
			/// @todo 读取子目录项，加载该node

			return nullptr;
		}
	}
}