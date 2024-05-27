#include "fs/dentry.hh"
#include "fs/fat/fat32inode.hh"
#include "fs/fat/fat32.hh"

#include <EASTL/string.h>
#include <EASTL/unordered_map.h>

namespace fs
{
	class Inode;

	namespace fat
	{

		class Fat32Inode;
		class Fat32SuperBlock;
		class Fat32DirectryShort;

		using cluster_num_t = uint;

		class Fat32Dentry : public Dentry
		{
		private:
			Fat32Dentry *_parent;
			Fat32Inode *_node;
			eastl::unordered_map<eastl::string, Fat32DirectryShort> _children;
			// eastl::unordered_map<eastl::string, Fat32Dentry *> _dentry_children;
			eastl::string _name;
			Dentry * _sub_dir_cache = nullptr;
		public:

			Fat32Dentry() = default;
			Fat32Dentry( const Fat32Dentry& dentry ) = delete;
			Fat32Dentry & operator=( const Fat32Dentry& dentry ) = delete;
			Fat32Dentry( Fat32Dentry *parent_, Fat32Inode *node_, eastl::string name_ ) : _parent( parent_ ), _node( node_ ), _name( name_ ) {};
			~Fat32Dentry();
			virtual Dentry * EntrySearch( eastl::string name ) override;
			virtual Dentry * EntryCreate( eastl::string name, uint32 mode ) override { return nullptr; };
			virtual Inode * getNode() override { return _node; };
			virtual bool isRoot() override { return false; };
			virtual Dentry *getParent() override { return _parent == nullptr ? nullptr : _parent; };
			virtual eastl::string getName() override { return _name; };
			virtual bool isMntPoint() override { return false; };
			Fat32Dentry *dirinfo2dentry( Fat32Dentry *dirinfo );
			void rootInit( Fat32Inode *node ) { _parent = nullptr; _node = node; _name = "/"; };
			void init( uint32 dev, Fat32SuperBlock *sb, Fat32FS *fs );   /// @todo init root
			virtual int dentry_type() override { return -1; };

		};
	}
}