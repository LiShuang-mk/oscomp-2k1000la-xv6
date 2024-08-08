//
// Created by Li Shuang ( pseudonym ) on 2024-05-16 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "types.hh"

namespace fs
{
	class DStat
	{
	public:
		uint64 d_ino;   //index of inode
		int64 d_off;    //offset
		uint16 d_reclen;    //current direntry length
		uint8 d_type;   //file type
		char d_name[ 256 ];  //file name

		DStat() = default;
		DStat( const DStat& ds )
			: d_ino( ds.d_ino )
			, d_off( ds.d_off )
			, d_reclen( ds.d_reclen )
			, d_type( ds.d_type )
		{
			strcpy( d_name, ds.d_name );
		};
		DStat( uint64 ino, int64 off, uint16 reclen, uint8 type, char *name )
			: d_ino( ino )
			, d_off( off )
			, d_reclen( reclen )
			, d_type( type )
		{
			strcpy( d_name, name );
		};
		~DStat() = default;
	};
} // namespace fs
