#pragma once


#include "fs/fat/fat32.hh"
#include "fs/fs_defs.hh"
#include "fs/fs.hh"

namespace fs{

	class dentry;
	class Inode;
    namespace fat{
		
		//FileAttrs defaultAttrs = FileAttrs( 0777 );
		class Fat32SuperBlock : public SuperBlock
		{
			private:
				FileSystem *_fs;
				fat::Fat32Dbr _dbr;
				bool is_valid;
				uint32 ino = 0;
			public:
				Fat32SuperBlock() = default;
				Fat32SuperBlock(const Fat32SuperBlock &super_block) = default;
				Fat32SuperBlock( FileSystem *fs, fat::Fat32Dbr *dbr, bool valid)
					: _fs(fs), _dbr(*dbr), is_valid(valid) { ino = 0; };
				~Fat32SuperBlock() = default;

				Fat32SuperBlock& operator= (const Fat32SuperBlock &super_block) = default;
				dentry * getRoot() const override { return _fs->getRoot(); }
				dentry * getMntPoint() const override { return _fs->getMntPoint(); }
				FileSystem * getFileSystem() const override { return _fs; }
				bool isValid() const override { return is_valid; }
				FileAttrs rDefaultMod() const override { return FileAttrs( 0777 ); }
				fat::FatBpb * get_bpb() { return &_dbr.bpb; }
				fat::Fat32Ebpb * get_ebpb() { return &_dbr.ebpb; }
				fat::Fat32Dbr * get_dbr() { return &_dbr; }
				size_t rBlockSize() const override { return _dbr.bpb.bytes_per_sector; }
				uint rSectorPClu() { return _dbr.bpb.sectors_per_cluster; }
				uint rFatCnt() { return _dbr.bpb.table_count; }
				size_t rBlockNum() const override { return _dbr.bpb.total_sectors_32; }
				Inode *allocInode( FileAttrs attr, eastl::string dev_name = "" ) override ;
		};
    }
}