//
// Created by Li Shuang ( pseudonym ) on 2024-05-16 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/fat/fat32_dir_entry.hh"
#include "fs/fat/fat32_file_system.hh"
#include "fs/buffer_manager.hh"
#include "klib/common.hh"

namespace fs
{
	namespace fat
	{
		Fat32DirEntry::Fat32DirEntry()
			: _belong_fs( nullptr )
			, _dir_type( fat32de_unkown )
			, _children( eastl::unordered_map<eastl::string, Fat32DirectryShort>() )
		{
			log_info( "Fat32DirEntry constructor invoked." );
		}

		Fat32DirEntryStatus Fat32DirEntry::init( uint cluster_number, Fat32DirType dir_type )
		{
			assert( _belong_fs != nullptr, "Fat32DirEntry: no belonging fs deteted while init. cluster number: %d", cluster_number );
			uint dev = _belong_fs->owned_device();
			Fat32Dbr * dbr = _belong_fs->get_super_block();
			assert( dbr->bootable_signature = 0xAA55, "Fat32DirEntry: get invalid DBR while init." );

			_clusters_number.clear();
			// delete _children;
			// _children = new eastl::unordered_map<eastl::string, Fat32DirectryShort>;
			_children.clear();


			// read File Allocation table

			Buffer buf = k_bufm.read_sync(
				dev,
				_cluster_to_lba( cluster_number )
			);

			// record all clusters covered

			using fat32_t = uint32;
			fat32_t * fat32_p = ( fat32_t * ) buf.get_data_ptr();
			fat32_t * buf_end = ( fat32_t * ) buf.get_end_ptr();
			for ( fat32_t i = cluster_number; fat32_p < buf_end; fat32_p++ )
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

			// log_trace( "debug print root clusters number:" );
			// for ( fat32_t cls : _clusters_number )
			// 	printf( "%d ", cls );
			// printf( "\n" );
			// while ( 1 );

			_dir_type = dir_type;

			// if this is a folder, then read all sub-dir into memory

			if ( dir_type == fat32de_folder )
			{
				// TODO: refer to [1]
				for ( uint32 cls_num : _clusters_number )
				{
					uint64 data_lba = _cluster_to_lba( cls_num );
					buf = k_bufm.read_sync(
						dev,
						data_lba
					);
					void *desc_ptr = ( void * ) buf.get_data_ptr();
					while ( *( char * ) desc_ptr )
					{
						_read_sub_dir( desc_ptr );
					}
					k_bufm.release_buffer_sync( buf );
				}

				// log_trace( "debug print files under root." );
				// printf( "list /* \n" );
				// uint cnt = 0;
				// char padding[] = "                    ";
				// uint width = sizeof( padding ) - 1;
				// for ( auto it = _children.begin(); it != _children.end(); ++it, ++cnt )
				// {
				// 	uint rest = width - it->first.size();
				// 	padding[ rest ] = 0;
				// 	printf( "%s", it->first.c_str() );
				// 	printf( "%s", padding );
				// 	padding[ rest ] = ' ';
				// 	if ( cnt % 4 == 3 )
				// 		printf( "\n" );
				// }

				// log_trace( "debug print files start cluster." );
				// char padding[] = "                    ";
				// uint width = sizeof( padding ) - 1;
				// printf( "\033[91mfile-name           start-cluster ---- file-size\033[0m\n" );
				// for ( auto it = _children.begin(); it != _children.end(); ++it )
				// {
				// 	uint rest = width - it->first.size();
				// 	padding[ rest ] = 0;
				// 	printf( "%s", it->first.c_str() );
				// 	printf( "%s", padding );
				// 	padding[ rest ] = ' ';
				// 	uint64 cls_num = ( uint64 ) it->second.first_cluster_low + ( it->second.first_cluster_high << 16 );
				// 	uint64 fil_siz = ( uint64 ) it->second.file_size_bytes;
				// 	printf( "%d  (0x%x) ---- %d Bytes\n", cls_num, cls_num, fil_siz );
				// }
			}

			return Fat32DirEntryStatus::fat32de_init_ok;
		}


		Fat32DirEntryStatus Fat32DirEntry::find_sub_dir( eastl::string &dir_name, Fat32DirInfo &dir_info )
		{
			auto it = _children.find( dir_name );
			if ( it == _children.end() )
				return fat32de_find_subdir_fail;

			dir_info = it->second;
			return fat32de_find_subdir_success;
		}


		void Fat32DirEntry::read_content( void * buf, uint64 read_len, uint64 offset, bool async_read )
		{
			if ( _dir_type == fat32de_folder )
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
					memcpy( ( void * ) writ_head, disk_buf.get_data_ptr(), ( int ) default_buffer_size);
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


// -------- private helper function --------

		int Fat32DirEntry::_read_sub_dir( void * &sub_desc )
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

			// record children dir
			// _children.insert_or_assign( dir_name, *dirshort );
			_children[ dir_name ] = *dirshort;

			// move to next file-desc
			dirshort++;
			sub_desc = ( void* ) dirshort;

			return 0;
		}

		uint64 Fat32DirEntry::_cluster_to_lba( uint64 cluster )
		{
			return
				_belong_fs->start_lba() +	// fat32 start lba
				_belong_fs->get_reserved_sectors() +	// skip reserved sectors, including BPB
				_belong_fs->get_sectors_per_fat() * _belong_fs->get_fat_counts() +	// skip File Allocation Table
				_belong_fs->get_sectors_per_cluster() * ( cluster - Fat32_first_data_cluster );	// sector offset in DATA
		}

		uint64 Fat32DirEntry::_cover_size_bytes()
		{
			return
				_clusters_number.size()
				* _belong_fs->get_sectors_per_cluster()
				* _belong_fs->get_bytes_per_sector();
		}

	} // namespace fat

} // namespace fs
