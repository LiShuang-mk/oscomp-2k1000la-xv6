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
		Fat32DirEntryStatus Fat32DirEntry::init( uint cluster_number )
		{
			assert( _belong_fs != nullptr, "Fat32DirEntry: no belonging fs deteted while init. cluster number: %d", cluster_number );
			[[maybe_unused]] uint dev = _belong_fs->owned_device();
			[[maybe_unused]] uint64 start_addr = _belong_fs->start_lba();
			Fat32Dbr * dbr = _belong_fs->get_super_block();
			assert( dbr->bootable_signature = 0xAA55, "Fat32DirEntry: get invalid DBR while init." );

			Buffer buf = k_bufm.read_sync(
				dev,
				start_addr + dbr->ebpb.table_size * dbr->bpb.table_count + dbr->bpb.reserved_sector_count
			);
			using fat32_t = uint32;
			fat32_t * fat32_p = ( fat32_t* ) buf.get_data_ptr();
			for ( fat32_t i = 2; ; fat32_p++ )
			{
				_clusters_number.push_back( i );
				if ( fat_is_end( *fat32_p ) )
					break;
				i = *fat32_p;
			}

			// log_trace( "debug print root clusters number:" );
			// for ( fat32_t cls : _clusters_number )
			// 	printf( "%d ", cls );
			// printf( "\n" );
			// while ( 1 );

			return Fat32DirEntryStatus::fat32de_undefined;
		}

	} // namespace fat
	
} // namespace fs
