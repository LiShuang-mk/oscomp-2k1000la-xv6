#include "types.hh"
#include "fs/fat/fat32.hh"  
#include "fs/fat/fat32Sb.hh"
#include "fs/file/file.hh"

#include <EASTL/vector.h>
#include <EASTL/string.h>

namespace fs
{

	class SuperBlock;
	class dentry;
	namespace fat
	{

		class Fat32Dentry;

		constexpr size_t Fat32MaxFileNameLength = 255;
		constexpr uint64 Fat32MaxPathLength = 255;
		class Fat32FS : public FileSystem
		{
		private:
			uint _device;
			uint64 _start_lba;
			uint64 _fat_lba;
			eastl::string _fstype;
			fat::Fat32SuperBlock *_super_block;
			bool _isroot;

			dentry *_root;
			dentry *_mnt;
			//eastl::string key;			// map key for fs when mounts
		public:
			Fat32FS() = default;
			Fat32FS( const Fat32FS &fs ) = default;
			Fat32FS( bool isroot_, eastl::string key_ ) : _isroot( isroot_ ) {};
			Fat32FS( bool isroot_, dentry *mnt ) : _isroot( isroot_ ), _mnt( mnt ) {};
			~Fat32FS();
			void init( int dev, uint64 start_lba, eastl::string fstype, eastl::string rootname, bool is_root = false );

			dentry * get_root() { return _root; }
			dentry * get_dir_entry( Fat32DirInfo &dir_info );

			Fat32FS & operator= ( const Fat32FS &fs ) = default;
			//inline eastl::string rFSType() const override { return _fstype; }
			//inline eastl::string rKey() const override { return key; }
			inline bool isRootFS() const override { return _isroot; }
			inline SuperBlock * getSuperBlock() const override { return _super_block; }
			//int ldSuperBlock( uint64 dev, Dentry * mnt ) override { return -1; } // @todo FAT32 load super block
			long rMagic() const override { return _super_block->get_dbr()->bootable_signature; }/// @todo FAT32 magic number 
			size_t rBlockSize() const override { return _super_block->rBlockSize(); }
			long rBlockNum() const override { return _super_block->rBlockNum(); }
			long rBlockFree() const override { return rBlockNum(); } /// @todo FAT32 free block number
			long rMaxFile() const override { return rBlockNum() / _super_block->rSectorPClu(); } /// @todo FAT32 max file number
			long rFreeFile() const override { return rBlockFree() / _super_block->rSectorPClu(); } /// @todo FAT32 free file number
			eastl::string rFStype() const override { return _fstype; } 
			size_t rNamelen() const override { return Fat32MaxPathLength; }
			FileAttrs rDefaultMod() const override { return _super_block->rDefaultMod(); }
			//void unInstall() override {  } /// @todo FAT32 uninstall file system
			dentry *getRoot() const override ;
			dentry *getMntPoint() const override { return _mnt; };
			int mount( dentry *dev, dentry *mnt, eastl::string fstype )  override { return 0; }	
			int umount( dentry *mnt ) override ; // uncomplated
		public:
			uint owned_device() { return _device; }
			uint64 start_lba() { return _start_lba; }

			Fat32Dbr * get_dbr() { return _super_block->get_dbr(); }
			uint64 get_reserved_sectors() { return _super_block->get_bpb()->reserved_sector_count; }
			uint64 get_fat_lba() { return _fat_lba; }
			uint64 get_fat_counts() { return _super_block->get_bpb()->table_count; }
			uint64 get_sectors_per_fat() { return _super_block->get_ebpb()->table_size; }
			uint64 get_sectors_per_cluster() { return _super_block->get_bpb()->sectors_per_cluster; }
			uint64 get_bytes_per_sector() { return _super_block->get_bpb()->bytes_per_sector; }

			dentry * get_root_dir() { return _root; }

			//void DentryCacheTest();
		};

		extern Fat32FS k_fatfs;
	}
}