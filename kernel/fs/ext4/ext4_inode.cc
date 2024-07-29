//
// Created by Li Shuang ( pseudonym ) on 2024-07-23 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/ext4/ext4_inode.hh"
#include "fs/ext4/ext4_fs.hh"
#include "klib/template_algorithmn.hh"
#include "klib/klib.hh"


// <<<<<<<< hash tree 相关 
namespace fs
{
	namespace ext4
	{

/*
 * The following notice applies to the code in this region ( hash
 * tree ) except fs::ext4::ext4_half_md4() :
 *
 * Copyright (c) 2013 Grzegorz Kostka (kostka.grzegorz@gmail.com)
 *
 * FreeBSD:
 * Copyright (c) 2010, 2013 Zheng Liu <lz@freebsd.org>
 * Copyright (c) 2012, Vyacheslav Matyushin
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*
 * The following notice applies to the code in fs::ext4::ext4_half_md4():
 *
 * Copyright (C) 1990-2, RSA Data Security, Inc. All rights reserved.
 *
 * License to copy and use this software is granted provided that it
 * is identified as the "RSA Data Security, Inc. MD4 Message-Digest
 * Algorithm" in all material mentioning or referencing this software
 * or this function.
 *
 * License is also granted to make and use derivative works provided
 * that such works are identified as "derived from the RSA Data
 * Security, Inc. MD4 Message-Digest Algorithm" in all material
 * mentioning or referencing the derived work.
 *
 * RSA Data Security, Inc. makes no representations concerning either
 * the merchantability of this software or the suitability of this
 * software for any particular purpose. It is provided "as is"
 * without express or implied warranty of any kind.
 *
 * These notices must be retained in any copies of any part of this
 * documentation and/or software.
 */

#define EXT4_HASH_FUNCTION(name) ext4_hash_##name
#define EXT4_HTREE_EOF 0x7FFFFFFFUL

		/* F, G, and H are MD4 functions */

		constexpr u32 EXT4_HASH_FUNCTION( F )( u32 x, u32 y, u32 z ) { return ( x & y ) | ( ~x & z ); }
		constexpr u32 EXT4_HASH_FUNCTION( G )( u32 x, u32 y, u32 z ) { return ( x & y ) | ( x & z ) | ( y & z ); }
		constexpr u32 EXT4_HASH_FUNCTION( H )( u32 x, u32 y, u32 z ) { return x ^ y ^ z; }

		/* ROTATE_LEFT rotates x left n bits */
		constexpr u32 EXT4_HASH_FUNCTION( rotate_left )( u32 x, u32 n ) { return ( x << n ) | ( x >> ( 32 - n ) ); }

		/*
		* FF, GG, and HH are transformations for rounds 1, 2, and 3.
		* Rotation is separated from addition to prevent recomputation.
		*/

		constexpr void EXT4_HASH_FUNCTION( FF )( u32 &a, u32 &b, u32 &c, u32 &d, u32 &x, u32 s )
		{
			a += EXT4_HASH_FUNCTION( F )( b, c, d ) + x;
			a = EXT4_HASH_FUNCTION( rotate_left )( a, s );
		}

		constexpr void EXT4_HASH_FUNCTION( GG )( u32 &a, u32 &b, u32 &c, u32 &d, u32 &x, u32 s )
		{
			a += EXT4_HASH_FUNCTION( G )( b, c, d ) + x + ( u32 ) 0x5A827999;
			a = EXT4_HASH_FUNCTION( rotate_left )( a, s );
		}

		constexpr void EXT4_HASH_FUNCTION( HH )( u32 &a, u32 &b, u32 &c, u32 &d, u32 &x, u32 s )
		{
			a += EXT4_HASH_FUNCTION( H )( b, c, d ) + x + ( u32 ) 0x6ED9EBA1;
			a = EXT4_HASH_FUNCTION( rotate_left )( a, s );
		}

		/*
		* MD4 basic transformation.  It transforms state based on block.
		*
		* This is a half md4 algorithm since Linux uses this algorithm for dir
		* index.  This function is derived from the RSA Data Security, Inc. MD4
		* Message-Digest Algorithm and was modified as necessary.
		*
		* The return value of this function is u32 in Linux, but actually we don't
		* need to check this value, so in our version this function doesn't return any
		* value.
		*/
		static void ext4_half_md4( u32 hash[ 4 ], u32 data[ 8 ] )
		{
			u32 a = hash[ 0 ], b = hash[ 1 ], c = hash[ 2 ], d = hash[ 3 ];

			/* Round 1 */
			EXT4_HASH_FUNCTION( FF )( a, b, c, d, data[ 0 ], 3 );
			EXT4_HASH_FUNCTION( FF )( d, a, b, c, data[ 1 ], 7 );
			EXT4_HASH_FUNCTION( FF )( c, d, a, b, data[ 2 ], 11 );
			EXT4_HASH_FUNCTION( FF )( b, c, d, a, data[ 3 ], 19 );
			EXT4_HASH_FUNCTION( FF )( a, b, c, d, data[ 4 ], 3 );
			EXT4_HASH_FUNCTION( FF )( d, a, b, c, data[ 5 ], 7 );
			EXT4_HASH_FUNCTION( FF )( c, d, a, b, data[ 6 ], 11 );
			EXT4_HASH_FUNCTION( FF )( b, c, d, a, data[ 7 ], 19 );

			/* Round 2 */
			EXT4_HASH_FUNCTION( GG )( a, b, c, d, data[ 1 ], 3 );
			EXT4_HASH_FUNCTION( GG )( d, a, b, c, data[ 3 ], 5 );
			EXT4_HASH_FUNCTION( GG )( c, d, a, b, data[ 5 ], 9 );
			EXT4_HASH_FUNCTION( GG )( b, c, d, a, data[ 7 ], 13 );
			EXT4_HASH_FUNCTION( GG )( a, b, c, d, data[ 0 ], 3 );
			EXT4_HASH_FUNCTION( GG )( d, a, b, c, data[ 2 ], 5 );
			EXT4_HASH_FUNCTION( GG )( c, d, a, b, data[ 4 ], 9 );
			EXT4_HASH_FUNCTION( GG )( b, c, d, a, data[ 6 ], 13 );

			/* Round 3 */
			EXT4_HASH_FUNCTION( HH )( a, b, c, d, data[ 3 ], 3 );
			EXT4_HASH_FUNCTION( HH )( d, a, b, c, data[ 7 ], 9 );
			EXT4_HASH_FUNCTION( HH )( c, d, a, b, data[ 2 ], 11 );
			EXT4_HASH_FUNCTION( HH )( b, c, d, a, data[ 6 ], 15 );
			EXT4_HASH_FUNCTION( HH )( a, b, c, d, data[ 1 ], 3 );
			EXT4_HASH_FUNCTION( HH )( d, a, b, c, data[ 5 ], 9 );
			EXT4_HASH_FUNCTION( HH )( c, d, a, b, data[ 0 ], 11 );
			EXT4_HASH_FUNCTION( HH )( b, c, d, a, data[ 4 ], 15 );

			hash[ 0 ] += a;
			hash[ 1 ] += b;
			hash[ 2 ] += c;
			hash[ 3 ] += d;
		}

		/*
		* Tiny Encryption Algorithm.
		*/
		static void ext4_tea( u32 hash[ 4 ], u32 data[ 8 ] )
		{
			u32 tea_delta = 0x9E3779B9;
			u32 sum;
			u32 x = hash[ 0 ], y = hash[ 1 ];
			int n = 16;
			int i = 1;

			while ( n-- > 0 )
			{
				sum = i * tea_delta;
				x += ( ( y << 4 ) + data[ 0 ] ) ^ ( y + sum ) ^ ( ( y >> 5 ) + data[ 1 ] );
				y += ( ( x << 4 ) + data[ 2 ] ) ^ ( x + sum ) ^ ( ( x >> 5 ) + data[ 3 ] );
				i++;
			}

			hash[ 0 ] += x;
			hash[ 1 ] += y;
		}

		static u32 ext4_legacy_hash( const char *name, int len, int unsigned_char )
		{
			u32 h0, h1 = 0x12A3FE2D, h2 = 0x37ABE8F9;
			u32 multi = 0x6D22F5;
			const unsigned char *uname = ( const unsigned char * ) name;
			const signed char *sname = ( const signed char * ) name;
			int val, i;

			for ( i = 0; i < len; i++ )
			{
				if ( unsigned_char )
					val = ( unsigned int ) *uname++;
				else
					val = ( int ) *sname++;

				h0 = h2 + ( h1 ^ ( val * multi ) );
				if ( h0 & 0x80000000 )
					h0 -= 0x7FFFFFFF;
				h2 = h1;
				h1 = h0;
			}

			return ( h1 << 1 );
		}

		static void ext4_prep_hashbuf(
			const char *src,
			u32 slen,
			u32 *dst,
			int dlen,
			int unsigned_char )
		{
			u32 padding = slen | ( slen << 8 ) | ( slen << 16 ) | ( slen << 24 );
			u32 buf_val;
			int len, i;
			int buf_byte;
			const unsigned char *ubuf = ( const unsigned char * ) src;
			const signed char *sbuf = ( const signed char * ) src;

			if ( slen > ( u32 ) dlen )
				len = dlen;
			else
				len = slen;

			buf_val = padding;

			for ( i = 0; i < len; i++ )
			{
				if ( unsigned_char )
					buf_byte = ( unsigned int ) ubuf[ i ];
				else
					buf_byte = ( int ) sbuf[ i ];

				if ( ( i % 4 ) == 0 )
					buf_val = padding;

				buf_val <<= 8;
				buf_val += buf_byte;

				if ( ( i % 4 ) == 3 )
				{
					*dst++ = buf_val;
					dlen -= sizeof( u32 );
					buf_val = padding;
				}
			}

			dlen -= sizeof( u32 );
			if ( dlen >= 0 )
				*dst++ = buf_val;

			dlen -= sizeof( u32 );
			while ( dlen >= 0 )
			{
				*dst++ = padding;
				dlen -= sizeof( u32 );
			}
		}

		[[maybe_unused]]
		static int ext4_htree_hash(
			const char *name,
			int len,
			const u32 *hash_seed,
			int hash_version,
			u32 *hash_major,
			u32 *hash_minor )
		{
			u32 hash[ 4 ];
			u32 data[ 8 ];
			u32 major = 0, minor = 0;
			int unsigned_char = 0;

			if ( !name || !hash_major )
				return ( -1 );

			if ( len < 1 || len > 255 )
				goto ext4_hash_error;

			hash[ 0 ] = 0x67452301;
			hash[ 1 ] = 0xEFCDAB89;
			hash[ 2 ] = 0x98BADCFE;
			hash[ 3 ] = 0x10325476;

			if ( hash_seed )
			{
				hash[ 0 ] = hash_seed[ 0 ];
				hash[ 1 ] = hash_seed[ 1 ];
				hash[ 2 ] = hash_seed[ 2 ];
				hash[ 3 ] = hash_seed[ 3 ];
			}

			using HashVer = Ext4DxRoot::_hash_version_enum;

			switch ( ( HashVer ) ( u8 ) hash_version )
			{
				case HashVer::u_tea:
					unsigned_char = 1;
					/* FALLTHRU */
				case HashVer::tea:
					while ( len > 0 )
					{
						ext4_prep_hashbuf( name, len, data, 16, unsigned_char );
						ext4_tea( hash, data );
						len -= 16;
						name += 16;
					}
					major = hash[ 0 ];
					minor = hash[ 1 ];
					break;
				case HashVer::u_legacy:
					unsigned_char = 1;
					/* FALLTHRU */
				case HashVer::legacy:
					major = ext4_legacy_hash( name, len, unsigned_char );
					break;
				case HashVer::u_half_md4:
					unsigned_char = 1;
					/* FALLTHRU */
				case HashVer::half_md4:
					while ( len > 0 )
					{
						ext4_prep_hashbuf( name, len, data, 32, unsigned_char );
						ext4_half_md4( hash, data );
						len -= 32;
						name += 32;
					}
					major = hash[ 1 ];
					minor = hash[ 2 ];
					break;
				default:
					goto ext4_hash_error;
			}

			major &= ~1;
			if ( major == ( EXT4_HTREE_EOF << 1 ) )
				major = ( EXT4_HTREE_EOF - 1 ) << 1;
			*hash_major = major;
			if ( hash_minor )
				*hash_minor = minor;

			return 0;

		ext4_hash_error:
			*hash_major = 0;
			if ( hash_minor )
				*hash_minor = 0;
			return -1;
		}

	} // namespace ext4

} // namespace fs
// >>>>>>>> hash tree 相关


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
			//_attrs( 0777 );
		}

		SuperBlock * Ext4IndexNode::getSb() const
		{
			return _belong_fs->getSuperBlock();
		}

		Inode * Ext4IndexNode::lookup( eastl::string dirname )
		{
			if ( !( _inode.mode & ext4_imode_fdir ) )		// 当前inode不是目录
			{
				log_warn( "try to lookup an inode that's not dir" );
				return nullptr;
			}

			if ( dirname == "." ) return this;

			if ( _inode.flags.fl.index )		// 当前目录使用 hash tree
			{
				return _htree_lookup( dirname );
			}
			else								// 当前目录使用 linear 结构
			{
				Ext4Buffer * dirent_buf;
				Inode * res_node;

				for ( long i = 0; i < _has_blocks; ++i )			// 顺序遍历 linear 目录项
				{
					dirent_buf = read_logical_block( i, true );		// pin the buffer
					if ( dirent_buf == nullptr )
					{
						log_error(
							"ext4-inode : 访问目录\"%s\"失败\n"
							"             访问失败的逻辑块号 %d",
							dirname.c_str(), i );
						return nullptr;
					}

					res_node = _linear_lookup( dirname, dirent_buf->get_data_ptr() );
					dirent_buf->unpin();

					if ( res_node != nullptr )
						return res_node;

					// 当前块内不包含目标目录项，准备读取下一个块

					dirent_buf->unpin();
				}
				// 遍历所有块均不包含目录项
			}

			return nullptr;
		}

		size_t fs::ext4::Ext4IndexNode::nodeRead( u64 dst, size_t off, size_t len )
		{
			if ( !( _inode.mode & ext4_imode_freg ) )
			{
				log_warn(
					"ext4-inode:\n"
					"\ttry to read inode that's not regular file\n"
					"\tnone to read out" );
				return 0;
			}
			if ( off >= ( size_t ) _has_size )
				return 0;

			long b_siz = _belong_fs->rBlockSize();			// 块大小
			size_t read_len =								// 需要读取的长度
				( off + len <= ( size_t ) _has_size ) ? len : ( ( size_t ) _has_size - off );

			u8 * d = ( u8* ) dst;							// 目标地址
			long block_no = off / b_siz;					// 起始数据块
			long b_idx = 0;									// 数据块索引
			u8 * f;											// 数据源地址
			long b_off;										// 块内偏移

			Ext4Buffer * blk_buf = read_logical_block( block_no, true );
			if ( blk_buf == nullptr )
			{
				log_warn( "ext4-inode : read logical block %d fail", block_no );
				return 0;
			}
			f = ( u8* ) blk_buf->get_data_ptr();
			for ( size_t i = 0; i < read_len; i++ )
			{
				b_off = ( off + i ) % b_siz;
				if ( b_off == 0 )
				{
					if ( blk_buf != nullptr )
						blk_buf->unpin();				// unpin the buffer last read
					blk_buf = read_logical_block( block_no + b_idx, true ); // pin the buffer
					if ( blk_buf == nullptr )
					{
						log_warn( "ext4-inode : read logical block %d fail", block_no + b_idx );
						return 0;
					}
					f = ( u8* ) blk_buf->get_data_ptr();
					b_idx++;
				}
				d[ i ] = f[ b_off ];
			}

			if ( blk_buf != nullptr )
				blk_buf->unpin();

			return read_len;
		}

		Ext4Buffer * Ext4IndexNode::read_logical_block( long block, bool pin )
		{
			if ( _inode.flags.fl.extents )					// 使用extents方式索引
			{
				using ex_node = Ext4Inode::_block_u_t_::_extent_s_t_::_node_u_t_;

				Ext4ExtentHeader * ex_header = &_inode.blocks.extents.header;	// extent 头
				ex_node * st, * ed;												// (union) extent 中间节点或叶子节点
				st = &( _inode.blocks.extents.tree_nodes[ 0 ] );				// 节点数组起始元素
				ed = st + _inode.blocks.extents.header.valid_nodes_count - 1;	// 节点数组末尾元素

				Ext4Buffer * internal_block = nullptr;							// exTree 中间块的 buffer

				// 叶节点二分查找比较函数
				auto comp_leaf = [] ( Ext4ExtentLeafNode * mid, long * tar ) -> int
				{
					long t = *tar;
					long l = mid->logical_block_start, r = l + mid->length;
					if ( l <= t && t < r )
						return 0;
					else if ( t < l )
						return -1;
					else
						return 1;
				};
				// 中间节点二分查找比较函数
				auto comp_intr = [ & ] ( Ext4ExtentInternalNode * mid, long * tar ) -> int
				{
					long t = *tar;
					if ( t < mid->logical_block_start )
						return -1;
					if ( mid != &( ed->internal ) && t >= ( mid + 1 )->logical_block_start )
						return 1;
					else
						return 0;
				};

				while ( 1 )		// 查找 extent 树
				{
					if ( ex_header->depth == 0 )	// 已经到达叶节点
					{
						// 二分查找

						Ext4ExtentLeafNode * dst_node =
							klib::binary_search<Ext4ExtentLeafNode, long>(
								&( st->leaf ), &( ed->leaf ), &block, comp_leaf
							);
						if ( dst_node == nullptr )		// 查找失败
						{
							log_error( "ext4-inode : extents leaf binary search error" );
							if ( internal_block )			// 返回前先将前一个块释放
								internal_block->unpin();
							return nullptr;
						}

						// 目标块的物理块号
						long phy_block = dst_node->start_lo + ( ( long ) dst_node->start_hi << 32 );
						phy_block += block - dst_node->logical_block_start;

						if ( internal_block )			// 返回前先将前一个块释放
							internal_block->unpin();

						return _belong_fs->read_block( phy_block, pin );
					}
					else							// 这是中间节点
					{
						// 二分查找

						Ext4ExtentInternalNode * next_node =
							klib::binary_search<Ext4ExtentInternalNode, long>(
								&( st->internal ), &( ed->internal ), &block, comp_intr
							);
						if ( next_node == nullptr )		// 查找失败
						{
							log_error( "ext4-inode : extents internal biary search error" );
							if ( internal_block )			// 返回前先将前一个块释放
								internal_block->unpin();
							return nullptr;
						}

						long next_block = next_node->next_node_address;						// 下一个节点所在块号

						if ( internal_block )			// 读下一个块之前先将前一个块释放
							internal_block->unpin();

						internal_block = _belong_fs->read_block( next_block, true );
						ex_header = ( Ext4ExtentHeader * ) internal_block->get_data_ptr();
						st = ( ex_node * ) ( ex_header + 1 );
						ed = st + ex_header->valid_nodes_count - 1;
					}
				}
			}
			else											// 使用经典的直接/间接索引
			{
				if ( block < 12 )		// 直接索引
					return _belong_fs->read_block( _inode.blocks.blocks_ptr[ block ], pin );
				Ext4Buffer * buf;
				if ( block > _belong_fs->get_tind_block_start() )	// 三级索引
				{
					buf = _belong_fs->read_block( _inode.blocks.blocks_ptr[ tind_blocks_idx ], true );
					Ext4Buffer * res = _search_tindirect_block(
						block,
						_inode.blocks.blocks_ptr[ tind_blocks_idx ],
						buf->get_data_ptr(),
						pin
					);
					buf->unpin();
					return res;
				}
				if ( block > _belong_fs->get_dind_block_start() )	// 二级索引
				{
					buf = _belong_fs->read_block( _inode.blocks.blocks_ptr[ dind_blocks_idx ], true );
					Ext4Buffer * res = _search_dindirect_block(
						block,
						_inode.blocks.blocks_ptr[ dind_blocks_idx ],
						buf->get_data_ptr(),
						pin
					);
					buf->unpin();
					return res;
				}
				if ( block > _belong_fs->get_dind_block_start() )	// 一级索引
				{
					buf = _belong_fs->read_block( _inode.blocks.blocks_ptr[ sind_blocks_idx ], true );
					Ext4Buffer * res = _search_sindirect_block(
						block,
						_inode.blocks.blocks_ptr[ sind_blocks_idx ],
						buf->get_data_ptr(),
						pin
					);
					buf->unpin();
					return res;
				}
				log_error( "ext4-inode : 无法寻址的逻辑块号 %d", block );
				return nullptr;
			}
		}


		void Ext4IndexNode::_cal_blocks()
		{
			Ext4SuperBlock * ext4_sb = _belong_fs->get_ext4_superblock();
			if ( ext4_sb->feature_ro_compat.flags.huge_file )	// 启用 blocks_high
			{
				if ( _inode.flags.fl.huge_file )	// inode 中的 blocks 是标准 block 的数量
				{
					_has_blocks = _inode.blocks_lo + ( ( long ) _inode.blocks_high << 32 );
				}
				else	// inode 中的 blocks 是 512-bytes block 的数量
				{
					_has_blocks = _inode.blocks_lo + ( ( long ) _inode.blocks_high << 32 );
					_has_blocks *= 512;
					_has_blocks /= _belong_fs->rBlockSize();
				}
			}
			else	// inode 中的 blocks 是 512-bytes block 的数量
			{
				_has_blocks = _inode.blocks_lo * 512;
				_has_blocks /= _belong_fs->rBlockSize();
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

		Ext4Buffer * Ext4IndexNode::_search_direct_block(
			long target_block_no,
			long start_block_no,
			void * index_block,
			bool pin )
		{
			u32 * p_block = ( u32 * ) index_block;
			p_block += target_block_no - start_block_no;
			return _belong_fs->read_block( *p_block, pin );
		}

		Ext4Buffer * Ext4IndexNode::_search_sindirect_block(
			long target_block_no,
			long start_block_no,
			void * index_block,
			bool pin )
		{
			u32 * p_block = ( u32 * ) index_block;
			p_block += _cal_sind_index( target_block_no, start_block_no );
			Ext4Buffer * buf = _belong_fs->read_block( *p_block, true );
			Ext4Buffer * res = _search_direct_block( target_block_no, *p_block, buf->get_data_ptr(), pin );
			buf->unpin();
			return res;
		}

		Ext4Buffer * Ext4IndexNode::_search_dindirect_block(
			long target_block_no,
			long start_block_no,
			void * index_block,
			bool pin )
		{
			u32 * p_block = ( u32 * ) index_block;
			p_block += _cal_dind_index( target_block_no, start_block_no );
			Ext4Buffer * buf = _belong_fs->read_block( *p_block, true );
			Ext4Buffer * res = _search_sindirect_block( target_block_no, *p_block, buf->get_data_ptr(), pin );
			buf->unpin();
			return res;
		}

		Ext4Buffer * Ext4IndexNode::_search_tindirect_block(
			long target_block_no,
			long start_block_no,
			void * index_block,
			bool pin )
		{
			u32 * p_block = ( u32 * ) index_block;
			p_block += _cal_tind_index( target_block_no, start_block_no );
			Ext4Buffer * buf = _belong_fs->read_block( *p_block, true );
			Ext4Buffer * res = _search_dindirect_block( target_block_no, *p_block, buf->get_data_ptr(), pin );
			buf->unpin();
			return res;
		}

		long Ext4IndexNode::_cal_tind_index( long target_block, long start_block )
		{
			long unit = _belong_fs->rBlockSize();
			unit *= unit * unit;		// block_size ^ 3
			return ( target_block - start_block ) / unit;
		}
		long Ext4IndexNode::_cal_dind_index( long target_block, long start_block )
		{
			long unit = _belong_fs->rBlockSize();
			unit *= unit;				// block_size ^ 2
			return ( target_block - start_block ) / unit;
		}
		long Ext4IndexNode::_cal_sind_index( long target_block, long start_block )
		{
			long unit = _belong_fs->rBlockSize();
			return ( target_block - start_block ) / unit;
		}

		Inode * Ext4IndexNode::_htree_lookup( eastl::string &dir_name )
		{
			if ( dir_name == ".." )
			{
				log_error(
					"ext4-inode : lookup parent directory?\n"
					"             but vfs-inode didn't known how to find parent\n"
					"             maybe it should be found in Dentry."
				);
				return nullptr;
			}

			Ext4Buffer * block_buf = read_logical_block( 0, true );			// read root of htree
			Ext4DxRoot * dxroot = ( Ext4DxRoot * ) block_buf->get_data_ptr();

			if ( block_buf == nullptr )
			{
				log_error( "ext4-inode : read logical block 0 fail" );
				return nullptr;
			}

			// 计算 dir_name 的 hash value

			u32 hash_seed[ 4 ];							// 哈希种子在超级块中
			_belong_fs->get_hash_seed( hash_seed );
			u32 hash_major;
			u32 hahs_minor;
			if ( ext4_htree_hash(
				dir_name.c_str(), dir_name.size(),
				hash_seed, ( int ) dxroot->info.hash_version,
				&hash_major, &hahs_minor
			) < 0 )
			{
				log_error( "ext4-hash fail for dir-name : %s", dir_name.c_str() );
				block_buf->unpin();
				return nullptr;
			}

			// 搜索 Hash Tree

			int ind_lvl = dxroot->info.indirect_levels;		// 间接搜索等级（树深度）
			Ext4DxEntry * st, * ed;
			Ext4DxNode * nd;
			st = dxroot->entries;
			ed = st + dxroot->count - 2;					// count 比实际上的数组长度多一，因为包含 header

			// 二分查找比较函数
			auto comp = [ & ] ( Ext4DxEntry * mid, u32 * tar ) -> int
			{
				u32 hash = *tar;
				if ( hash < mid->hash )	return -1;
				if ( mid == ed )		return 0;
				else if ( mid < ed )
				{
					if ( hash >= ( mid + 1 )->hash ) return 1;
					else return 0;
				}
				else
				{
					log_panic( "binary search : bad mid beyond the end of array" );
					return 0;
				}
			};

			for ( ; ind_lvl >= 0; ind_lvl-- )
			{
				// 二分查找

				Ext4DxEntry * target_ent = klib::binary_search<Ext4DxEntry, u32>(
					st, ed, &hash_major, comp
				);
				if ( target_ent == nullptr )
				{
					log_error( "binary search fail" );
					block_buf->unpin();
					return nullptr;
				}

				// 目标子树块号
				long target_block = target_ent->block;
				block_buf->unpin();

				block_buf = _belong_fs->read_block( target_block, true );
				nd = ( Ext4DxNode * ) block_buf->get_data_ptr();
				st = nd->entries;
				ed = st + nd->count - 2;
			}

			// 达到叶子节点（数据节点），采用 linear 搜索

			Inode * res_node = _linear_lookup( dir_name, block_buf->get_data_ptr() );
			block_buf->unpin();

			return res_node;
		}

		Inode * Ext4IndexNode::_linear_lookup( eastl::string &dir_name, void * block )
		{
			u8 * idx;										// 块内字节索引
			Ext4DirEntry2 * dirent;

			idx = ( u8 * ) block;
			dirent = ( Ext4DirEntry2 * ) idx;

			while ( 1 )
			{
				// 无效目录项，已到达当前块末尾
				if ( dirent->inode == 0 )
					break;

				// 长度相等，可能是匹配项
				if ( dir_name.size() == dirent->name_len )
				{
					if ( strncmp( dir_name.c_str(), ( const char * ) dirent->name, dirent->name_len ) == 0 )
					{	// 匹配到目录项

						Ext4Inode dirent_inode;
						if ( _belong_fs->read_inode( dirent->inode, dirent_inode ) < 0 )
						{	// 读取子目录项inode失败
							log_error(
								"ext4-inode : read sub-inode fail\n"
								"             sub-inode-no=%d", dirent->inode
							);
							return nullptr;
						}

						// 读取inode成功，返回inode

						Ext4IndexNode * sub_inode = new Ext4IndexNode( dirent_inode, _belong_fs );
						if ( sub_inode == nullptr ) log_warn( "ext4-inode : no mem to create sub-inode" );
						return sub_inode;
					}
				}

				// 不匹配目录项，移动到下一个目录项进行匹配

				idx += dirent->rec_len;
				dirent = ( Ext4DirEntry2 * ) idx;
			}
			// 没有匹配到目录项
			return nullptr;
		}

		void Ext4IndexNode::debug_hash( eastl::string dir_name )
		{
			using HashVer = Ext4DxRoot::_hash_version_enum;
			u32 hash_seed[ 4 ];
			_belong_fs->get_hash_seed( hash_seed );
			u32 hmajor;
			u32 hminor;
			HashVer hv = HashVer::half_md4;
			ext4_htree_hash( dir_name.c_str(), dir_name.size(),
				hash_seed, ( int ) hv, &hmajor, &hminor );
			printf( GREEN_COLOR_PRINT "hash dir \"%s\" = 0x%x-0x%x\n" CLEAR_COLOR_PRINT,
				dir_name.c_str(), hmajor, hminor );
		}

	} // namespace ext4

} // namespace fs
