
#include <EASTL/string.h>
#include <EASTL/unordered_map.h>
#include <EASTL/vector.h>

#include "fs/fat/fat32.hh"
#include "fs/inode.hh"

namespace fs
{
	namespace fat
	{
		class Fat32FS;

		enum Fat32NodeType
		{
			fat32nod_fatunkown,
			fat32nod_file,
			fat32nod_folder
		};

		class Fat32Inode : public Inode
		{
			friend Fat32FS;
			using cluster_num_t = uint;

		private:

			// uint32 _device;
			uint32				  _first_cluster;
			Fat32FS				 *_belong_fs;
			Fat32NodeType		  _dir_type;
			eastl::vector<uint32> _clusters_number;
			// eastl::unordered_map<eastl::string, Fat32DirectryShort>
			// _children;
			FileAttrs			  _attr;
			size_t				  _size;
			uint				  _ino;
			eastl::string		  _dev_name;
			Fat32Inode			 *_sub_inode_cache = nullptr;

		public:

			Fat32Inode() = default;
			Fat32Inode( Fat32FS *fs_, fs::FileAttrs attr, uint ino_,
						eastl::string dev_name = "" )
				: _belong_fs( fs_ )
				, _attr( attr )
				, _ino( ino_ )
				, _dev_name( dev_name ) {};
			void init( uint32 first_cluster, Fat32FS *belong_fs,
					   Fat32NodeType node_type, fs::FileAttrs attr,
					   size_t size = 0, eastl::string dev_name = "" );

			void read_content( void *buf, uint64 read_len, uint64 offset,
							   bool async_read = false );

			bool is_dir() { return _dir_type == fat32nod_folder; }

		public:

			void debug_set_belong_fs( Fat32FS *fs ) { _belong_fs = fs; }

		public:

			/**************************************************
			 * TODO:
			 * [1] 此处对于目录类型是文件夹而言是有隐含的bug的，
			 * 由于buffer的大小是一个页面，如果文件夹下的内容不
			 * 能够用一个页面装下，就会发生buffer越界，从而可能
			 * 导致读取到错误的内容。实际上，如果有一个目录项（
			 * 特别是长文件名的目录项）跨越了两个buffer，那么在
			 * 调用后面的 _read_sub_dir 时就会发生越界。
			 * [tofix] 考虑将跨越两个buffer的目录项单独处理，或
			 * 是对_read_sub_dir 进行修复
			 **************************************************/
			Inode *lookup( eastl::string dirname ) override;

			Inode *mknode( eastl::string name, FileAttrs attrs,
						   eastl::string dev_name = "" ) override;
			size_t nodeRead( uint64 dst_, size_t off_, size_t len_ ) override
			{
				read_content( (void *) dst_, len_, off_ );
				return len_;
			};
			size_t nodeWrite( uint64 src_, size_t off_, size_t len_ ) override
			{
				return 0;
			};
			FileAttrs	rMode() const override { return _attr; };
			dev_t		rDev() const override;
			uint64		rIno() const override { return _first_cluster; };
			size_t		rFileSize() const override { return _size; };
			SuperBlock *getSb() const override;
			FileSystem *getFS() const override;
			int			readlinkat( char *buf, size_t len ) override
			{
				return 0;
			}; // not implemented

			virtual size_t readSubDir( ubuf &dst, size_t off ) override
			{
				return 0;
			}

		private:

			uint64 _cluster_to_lba( uint64 cluster );
			uint64 _cover_size_bytes();
			int	   _read_sub_dir( void				*&sub_dir_desc_ptr,
								  eastl::string		 &out_dir_name,
								  Fat32DirectryShort &out_dir_info );

		private:
		};
	} // namespace fat
} // namespace fs
