//
// Created by Li Shuang ( pseudonym ) on 2024-07-22 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/ext4/ext4_fs.hh"
#include "fs/buffer_manager.hh"
#include "klib/common.hh"

#include <hsai_global.hh>
#include <device_manager.hh>
#include <block_device.hh>

namespace fs
{
	namespace ext4
	{
		void Ext4FS::init( int dev, u64 start_lba, eastl::string fstype, eastl::string rootname, bool is_root )
		{
			// 获得 dev 设备的块大小

			hsai::VirtualDevice * vd = hsai::k_devm.get_device( ( uint ) dev );
			if ( vd->type() != hsai::dev_block )
			{
				log_error( "device %d is not a block device.", dev );
				return;
			}
			hsai::BlockDevice * bd = ( hsai::BlockDevice * ) vd;
			long blk_size = bd->get_block_size();

			// 通常块大小应当为 512 字节
			assert( blk_size == 512, "bad block size (%d bytes) from device %d", blk_size, dev );

			// 将 superblock 读出
			// ext4 的 superblock 位于分区偏移 1024 bytes

			Buffer buf;
			buf = k_bufm.read_sync( dev, 2 );
			new ( &_sb ) Ext4SB( ( Ext4SuperBlock * ) buf.get_data_ptr(), this );

		}

	} // namespace ext4

} // namespace fs
