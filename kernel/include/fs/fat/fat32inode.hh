
#include "fs/inode.hh"

#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <EASTL/unordered_map.h>

namespace fs{
    namespace fat{
        class Fat32FS;
        
        enum Fat32NodeType{
            fat32nod_fatunkown,
			fat32nod_file,
			fat32nod_folder
        };

        class Fat32Inode : public Inode
		{
			friend Fat32FS;
			using cluster_num_t = uint;
		    private:
                uint32 _device;
                uint32 _ino;
                Fat32FS *_belong_fs;
                Fat32NodeType _dir_type;
                eastl::vector<uint32> _clusters_number;
                //eastl::unordered_map<eastl::string, Fat32DirectryShort> _children;
                mode_t _attr;
            public:
                Fat32Inode() = default;
                
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
                void init( uint32 dev, uint32 ino, Fat32FS *fs, Fat32NodeType node_type, eastl::vector<uint32> clusters_number, mode_t attr )
                {
                    _device = dev;
                    _ino = ino;
                    _belong_fs = fs;
                    _dir_type = node_type;
                    _clusters_number = clusters_number;
                    _attr = attr;
                }
                void read_content( void *buf, uint64 read_len, uint64 offset, bool async_read = false );

                bool is_dir() { return _dir_type == fat32nod_folder; }
            public:
                void debug_set_belong_fs( Fat32FS* fs ) { _belong_fs = fs; }

            public:
                virtual Inode* lookup( eastl::string dirname, off_t off_ = 0 ) override ;
                virtual Inode* mknode( eastl::string name, mode_t mode ) override { return nullptr; };
                virtual size_t nodeRead( uint64 dst_, size_t off_, size_t len_ ) override { read_content( (void *)dst_, len_, off_ ); return len_; };
                virtual size_t nodeWrite( uint64 src_, size_t off_, size_t len_ ) override { return 0; };
                virtual mode_t rMode() const override { return _attr; };
                virtual dev_t rDev() const override { return _device; };
                virtual uint64 rIno() const override { return _ino; };
                virtual SuperBlock *getSb() const override ;
                virtual Fat32FS *getFS() const { return _belong_fs; };
            private:
                uint64 _cluster_to_lba( uint64 cluster );
                uint64 _cover_size_bytes();

            private:
            };
    }
}