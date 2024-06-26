#pragma once

#include "fs/dentry.hh"
#include "fs/fat/fat32.hh"
#include "fs/fs_defs.hh"
#include "fs/fs.hh"

namespace fs{

    namespace fat{

        static constexpr mode_t default_mode = 0x0777;

		class Fat32SuperBlock : public SuperBlock
		{
			private:
				FileSystem *fs;
				fat::Fat32Dbr _dbr;
				bool is_valid;
				
			public:
				Fat32SuperBlock() = default;
				Fat32SuperBlock(const Fat32SuperBlock &super_block) = default;
				Fat32SuperBlock( FileSystem *fs, fat::Fat32Dbr *dbr, bool valid)
					: fs(fs), _dbr(*dbr), is_valid(valid) { };
				~Fat32SuperBlock() = default;

				Fat32SuperBlock& operator= (const Fat32SuperBlock &super_block) = default;
				Dentry * getRoot() const override { return fs->getRoot(); }
				Dentry * getMntPoint() const override { return fs->getMntPoint(); }
				FileSystem * getFileSystem() const override { return fs; }
				bool isValid() const override { return is_valid; }
				uint32 rDefaultMod() const override { return default_mode; }
				fat::FatBpb * get_bpb() { return &_dbr.bpb; }
				fat::Fat32Ebpb * get_ebpb() { return &_dbr.ebpb; }
				fat::Fat32Dbr * get_dbr() { return &_dbr; }
				size_t rBlockSize() const override { return _dbr.bpb.bytes_per_sector; }
				uint rSectorPClu() { return _dbr.bpb.sectors_per_cluster; }
				uint rFatCnt() { return _dbr.bpb.table_count; }
				size_t rBlockNum() const override { return _dbr.bpb.total_sectors_32; }
				
		};
    }
}