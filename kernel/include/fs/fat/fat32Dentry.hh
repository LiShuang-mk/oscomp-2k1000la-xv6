#include "fs/dentry.hh"
#include "fs/fat/fat32inode.hh"
#include "fs/fat/fat32.hh"

#include <EASTL/string.h>
#include <EASTL/unordered_map.h>

namespace fs
{
	class Inode;

	namespace dentrycache{

		class dentryCache;

	}
	namespace fat
	{

		class Fat32Inode;
		class Fat32SuperBlock;
		class Fat32DirectryShort;

		using cluster_num_t = uint;

		class Fat32Dentry : public Dentry
		{
			friend class dentrycache::dentryCache;
		private:
			Fat32Dentry *_parent;
			Fat32Inode *_node;
			// eastl::unordered_map<eastl::string, Fat32DirectryShort> _children;
			eastl::unordered_map<eastl::string, fs::Dentry *> _dentry_children;
			eastl::string _name;
			// Dentry * _sub_dir_cache = nullptr;
			uint Did; // dentry id
		public:

			Fat32Dentry() = default;
			Fat32Dentry( const Fat32Dentry& dentry ) = delete;
			Fat32Dentry & operator=( const Fat32Dentry& dentry ) = default;
			Fat32Dentry( Fat32Dentry *parent_, Fat32Inode *node_, eastl::string name_ ) : _parent( parent_ ), _node( node_ ), _name( name_ ) {};
			~Fat32Dentry();

			/// TODO: 这个方法简单的返回 _sub_dir_cache，并且不保证将来仍然能够使用
			///       为了保证多个进程能同时使用，这里应该改为池技术分配 dentry 实体 
			Dentry * EntrySearch( eastl::string name ) override;

			Dentry * EntryCreate( eastl::string name, uint32 mode ) override;
			Inode * getNode() override { return _node; };
			bool isRoot() override { return false; };
			Dentry *getParent() override { return _parent == nullptr ? nullptr : _parent; };
			eastl::string getName() override { return _name; };
			bool isMntPoint() override { return false; };
			Fat32Dentry *dirinfo2dentry( Fat32Dentry *dirinfo );
			void rootInit( Fat32Inode *node, eastl::string rootname ) { _parent = nullptr; _node = node; _name = rootname; };
			void init( uint32 dev, Fat32SuperBlock *sb, Fat32FS *fs, eastl::string rootname );   /// @todo init root
			void init( Fat32Dentry *parent_, Fat32Inode *node_, eastl::string name_ ) { _parent = parent_; _node = node_; _name = name_; };
			DentryType dentry_type() override { return DentryType::FAT32_DENTRY; };
			eastl::string rName() override { return _name; }; // get dentry' name
			uint getDid() override { return Did; };
			void reset( eastl::vector<int> &bitmap ) override; 
			void delete_child( eastl::string name ) override { _dentry_children.erase( name ); };

			void dentrycachetest();
			eastl::unordered_map<eastl::string , fs::Dentry *>& getChildren() override ;

		};
	}
};