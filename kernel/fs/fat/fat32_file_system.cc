//
// Created by Li Shuang ( pseudonym ) on 2024-05-16 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/fat/fat32_file_system.hh"
#include "fs/buffer_manager.hh"
#include "klib/common.hh"

namespace fs
{
	namespace fat
	{
		void Fat32FileSystem::init( uint device, uint64 part_start_lba )
		{
			Buffer buf = k_bufm.read_sync( device, part_start_lba );
			_super_block = *( Fat32Dbr* ) buf.get_data_ptr();

			// Fat32Dbr * dbr = &_super_block;
			// log_trace(
			// 	"扇区大小:              %d bytes\n"
			// 	"簇大小:                %d sectors\n"
			// 	"保留扇区数:            %d\n"
			// 	"FAT 数量:              %d\n"
			// 	"根目录条目数量:        %d\n"
			// 	"硬盘介质类型:          %x\n"
			// 	"一个磁道的扇区数:      %d\n"
			// 	"磁头的数量:            %d\n"
			// 	"隐藏扇区数量:          %d\n"
			// 	"逻辑分区中的扇区数量:  %d\n"
			// 	"FAT大小:               %d sectors\n",
			// 	dbr->bpb.bytes_per_sector,
			// 	dbr->bpb.sectors_per_cluster,
			// 	dbr->bpb.reserved_sector_count,
			// 	dbr->bpb.table_count,
			// 	dbr->bpb.root_entry_count,
			// 	dbr->bpb.media_type,
			// 	dbr->bpb.sectors_per_track,
			// 	dbr->bpb.head_side_count,
			// 	dbr->bpb.hidden_sector_count,
			// 	dbr->bpb.total_sectors_32,
			// 	dbr->ebpb.table_size
			// );

			_device = device;
			_start_lba = part_start_lba;

			_root._belong_fs = this;
			_root.init( 2 );
		}
	} // namespace fat

} // namespace fs
