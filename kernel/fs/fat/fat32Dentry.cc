#include "fs/fat/fat32Dentry.hh"
#include "fs/fat/fat32Sb.hh"
#include "fs/buffer.hh"
#include "fs/buffer_manager.hh"
#include "fs/fat/fat32Sb.hh"
#include "fs/fat/fat32fs.hh"
//#include "fs/dentrycache.hh"

#include <EASTL/vector.h>
namespace fs
{
	namespace fat
	{
		Fat32Dentry::~Fat32Dentry()
		{
			// delete _sub_dir_cache;
			for ( auto &p : _dentry_children )
			{
				delete p.second;
			}
		}

		void Fat32Dentry::init( uint32 dev, Fat32SuperBlock *sb, Fat32FS *fs )
		{
			_node = new Fat32Inode();
			_node->init(
				2,
				fs,
				fat32nod_folder,
				0
			);

			rootInit( _node );
		}

		Dentry * Fat32Dentry::EntrySearch( eastl::string name )
		{
			log_trace( "Fat32Dentry::EntrySearch: name = %s", name.c_str() );
			auto it = _dentry_children.find( name );
			if ( it != _dentry_children.end() )
				return it->second;
			// if ( _sub_dir_cache != nullptr && _sub_dir_cache->getName() == name )
			// {
			// 	return _sub_dir_cache;
			// }
			if ( [[maybe_unused]] auto subnod = ( _node->lookup( name ) ) )
			{
				log_trace( "Fat32Dentry::EntrySearch: found inode" );
				/// TODO: return local viriable?
				Fat32Dentry * subdentry = new Fat32Dentry( this, ( Fat32Inode * ) subnod, name );
				_dentry_children[ name ] = subdentry;
				return subdentry;

				// delete _sub_dir_cache;
				// _sub_dir_cache = new Fat32Dentry( this, ( Fat32Inode* ) subnod, name );
				// return _sub_dir_cache;
			}
			return nullptr;

		}

		Dentry * Fat32Dentry::EntryCreate( eastl::string name, uint32 mode )
		{
			_dentry_children[ name ] = new Fat32Dentry( this, nullptr, name );
			return nullptr;
		}

		void Fat32Dentry::reset( eastl::vector<int> &bitmap )
		{
			bitmap[ Did ] = 0;
			_parent->delete_child( _name ); // delete from parent
			_parent = nullptr;
			_node = nullptr;
			_name.clear();
			for( auto &p : _dentry_children )
			{
				p.second->reset( bitmap );
			}
			_dentry_children.clear();

			Did = 0;
		}
	
		// void Fat32Dentry::dentrycachetest( )
		// {
		// 	log_info( "Fat32Dentry::dentrycachetest" );
		// 	fs::dentrycache::dentryCache &dc = fs::dentrycache::k_dentryCache;
		// 	Dentry *dentry = dc.alloDentry( DentryType::FAT32_DENTRY );
			
		// 	Fat32Dentry *f32dentry = static_cast<Fat32Dentry *>( dentry );
		// 	f32dentry->init( this, nullptr, "Level_1_Entry_1" );
		// 	chil
			
		// }
	}
}