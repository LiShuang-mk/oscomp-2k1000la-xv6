//
// Created by Li Shuang ( pseudonym ) on 2024-07-23 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/ext4/ext4_inode.hh"
#include "fs/ext4/ext4_fs.hh"

namespace fs
{
	namespace ext4
	{
		Ext4IndexNode::Ext4IndexNode( Ext4Inode &node, Ext4FS * fs )
			: _inode( node )
			, _belong_fs( fs )
		{
			_cal_blocks();
			_cal_size();
		}

		Inode * Ext4IndexNode::lookup( eastl::string dirname, off_t off )
		{
			if ( !( _inode.mode & ext4_imode_fdir ) )		// 当前inode不是目录
			{
				log_warn( "try to lookup an inode that's not dir" );
				return nullptr;
			}

			if ( _inode.flags.fl.extents )					// 使用extents方式索引
			{

			}
			else											// 使用经典直接/间接索引
			{

			}

			return nullptr;
		}


		void Ext4IndexNode::_cal_blocks()
		{
			if ( _inode.flags.fl.huge_file )
			{
				_has_blocks = _inode.blocks_lo + ( ( long ) _inode.blocks_high << 32 );
			}
			else
			{
				_has_blocks = _inode.blocks_lo;
			}
		}

		void Ext4IndexNode::_cal_size()
		{
			if ( _belong_fs->support64bit() )
			{
				_has_size = _inode.size_lo + ( ( long ) _inode.size_high << 32 );
			}
			else
			{
				_has_size = _inode.size_lo;
			}
		}

	} // namespace ext4

} // namespace fs
