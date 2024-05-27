#include "fs/fat/fat32inode.hh"
#include "fs/fat/fat32fs.hh"
#include "fs/buffer.hh"
#include "fs/buffer_manager.hh"
#include "fs/inode.hh"

#include <EASTL/vector.h>

namespace fs
{

	namespace fat
	{
		void Fat32Inode::init( uint32 first_cluster, Fat32FS *belong_fs, Fat32NodeType node_type, mode_t attr )
		{
			_first_cluster = first_cluster;
			_belong_fs = belong_fs;
			_dir_type = node_type;
			_attr = attr;

			uint cluster_num = _first_cluster;
			uint64 cls_num_in_sector = cluster_num /
				( _belong_fs->get_bytes_per_sector() / sizeof( cluster_num_t ) );
			uint64 cls_num_offset = cluster_num %
				( _belong_fs->get_bytes_per_sector() / sizeof( cluster_num_t ) );

			Buffer buf = k_bufm.read_sync(
				_belong_fs->owned_device(),
				_belong_fs->get_fat_lba() + cls_num_in_sector
			);

			// eastl::vector<uint32> clusters_num;
			_clusters_number.clear();

			// get all cluster numbers

			using fat32_t = uint32;
			fat32_t * fat32_p = ( fat32_t * ) buf.get_data_ptr();
			fat32_p += cls_num_offset;
			fat32_t * buf_end = ( fat32_t * ) buf.get_end_ptr();
			for ( fat32_t i = cluster_num; fat32_p < buf_end; fat32_p++ )
			{
				_clusters_number.push_back( i );
				if ( fat_is_end( *fat32_p ) )
					break;
				i = *fat32_p;
			}
			k_bufm.release_buffer_sync( buf );

			assert(
				fat32_p < buf_end,
				"Fat32DirEntry: too long directory that covers more than %d clusters.",
				default_buffer_size / sizeof( uint32 )
			);
		}

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

		dev_t Fat32Inode::rDev() const
		{
			return _belong_fs->owned_device();
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

		SuperBlock * Fat32Inode::getSb() const
		{
			return _belong_fs->getSuperBlock();
		}

		Inode * Fat32Inode::lookup( eastl::string dirname, off_t off_ )
		{
			if ( !is_dir() ) return nullptr;

			fs::Buffer buf;
			eastl::string name;
			Fat32DirectryShort dir_info;
			bool find = false;

			/// @todo 读取子目录项，加载该node
			// TODO: refer to [1]
			for ( uint32 cls_num : _clusters_number )
			{
				uint64 data_lba = _cluster_to_lba( cls_num );
				buf = k_bufm.read_sync(
					rDev(),
					data_lba
				);
				void *desc_ptr = ( void * ) buf.get_data_ptr();
				while ( *( char * ) desc_ptr )
				{
					name.clear();
					if ( _read_sub_dir( desc_ptr, name, dir_info ) < 0 )
					{
						log_warn( "[Fat32Inode] read sub-dir fail. sub-dir name = %s", dirname.c_str() );
						k_bufm.release_buffer_sync( buf );
						return nullptr;
					}
					/// TODO: if dirname == dir.name then break
					if ( name == dirname )
					{
						find = true;
						break;
					}
				}
				if ( find )
				{
					delete _sub_inode_cache;
					_sub_inode_cache = new Fat32Inode();
					_sub_inode_cache->init(
						( ( uint32 ) dir_info.first_cluster_high << 16 ) + ( uint32 ) dir_info.first_cluster_low,
						_belong_fs,
						( dir_info.attribute & Fat32DirectryAttribute::fat32_directry ) ? Fat32NodeType::fat32nod_folder : Fat32NodeType::fat32nod_file,
						0		/// TODO: inode attribute
					);
					if ( _sub_inode_cache == nullptr )
					{
						log_warn( "[Fat32Inode] no mem to create new inode." );
						k_bufm.release_buffer_sync( buf );
						return nullptr;
					}
					break;
				}
				k_bufm.release_buffer_sync( buf );
			}

			if ( find )
				return _sub_inode_cache;
			else
				return nullptr;
		}

// ================ private helper function ================

		int Fat32Inode::_read_sub_dir( void * &sub_desc, eastl::string &out_dir_name, Fat32DirectryShort &out_dir_info )
		{
			Fat32DirectryShort * dirshort;
			Fat32DirectryLong * dirlong;

			// get dir-short location
			dirlong = ( Fat32DirectryLong * ) sub_desc;
			while ( dirlong->attribute == fat32_longfilename )
				dirlong++;
			dirshort = ( Fat32DirectryShort * ) dirlong;

			// get check sum
			uchar chksum = Fat32DirShortChkSum( ( uchar * ) &dirshort->file_ext );

			eastl::string dir_name;

			// if no dir-long, save the short name as dir-name
			if ( dirlong == ( Fat32DirectryLong * ) sub_desc )
			{
				char * cp;
				// handle file name 
				cp = dirshort->file_ext.file_name;
				for ( int i = 0; i < 8 && ( cp[ i ] != 0 || cp[ i ] != ' ' ); ++i )
					dir_name.push_back( cp[ i ] );
				// handle file extension
				cp = dirshort->file_ext.extension;
				if ( *cp != 0 || *cp != ' ' )
				{
					dir_name.push_back( '.' );
					for ( int i = 0; i < 3; ++i )
						dir_name.push_back( cp[ i ] );
				}
			}
			else // save the long name as dir-name
			{
				// check if sub_desc is the last dir-long-desc
				if ( !( ( Fat32DirectryLong* ) sub_desc )->entry_seq.is_last )
				{
					log_error( "Fat32DirEntry: dir-entry ptr is not pointing at the last entry." );
					return -2;
				}

				dirlong--;
				for ( ; dirlong >= ( Fat32DirectryLong * ) sub_desc; dirlong-- )
				{
					if ( dirlong->checksum != chksum )
					{
						log_error( "Fat32DirEntry: read bad file name." );
						return -1;
					}
					using unicode_t = uint16;
					unicode_t uc;
					for ( uint i = 0; i < sizeof( dirlong->file_name_1 ) / sizeof( unicode_t ); ++i )
					{
						uc = dirlong->file_name_1[ i ];
						if ( uc == 0 || uc == 0xFFFFU )
							break;
						dir_name.push_back( unicode_to_ascii( uc ) );
					}
					for ( uint i = 0; i < sizeof( dirlong->file_name_2 ) / sizeof( unicode_t ); ++i )
					{
						uc = dirlong->file_name_2[ i ];
						if ( uc == 0 || uc == 0xFFFFU )
							break;
						dir_name.push_back( unicode_to_ascii( uc ) );
					}
					for ( uint i = 0; i < sizeof( dirlong->file_name_3 ) / sizeof( unicode_t ); ++i )
					{
						uc = dirlong->file_name_3[ i ];
						if ( uc == 0 || uc == 0xFFFFU )
							break;
						dir_name.push_back( unicode_to_ascii( uc ) );
					}
				}
			}

			out_dir_info = *dirshort;
			out_dir_name = dir_name;

			// move to next file-desc
			sub_desc = ( void* ) ( dirshort + 1 );

			return 0;
		}
	}
}