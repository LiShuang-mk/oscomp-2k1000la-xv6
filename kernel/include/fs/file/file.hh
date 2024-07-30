//
// Created by Li Shuang ( pseudonym ) on 2024-05-19 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "fs/file/file_defs.hh"
#include "fs/kstat.hh"

#include "pm/ipc/pipe.hh"

#include <EASTL/string.h>
namespace pm
{
	namespace ipc{

		class Pipe;

	}
}

using namespace pm::ipc;
namespace fs
{	
	class dentry;
	class file_pool;
	class File
	{
		friend file_pool;
	public:
		FileTypes type;
		int flags;
		FileOps ops;
		uint32 refcnt;
		union Data
		{
		private:
			struct { const FileTypes type_; Kstat kst_; } dv_;
			struct { const FileTypes type_; Kstat kst_; Pipe *pipe_; } pp_;
			struct { const FileTypes type_; Kstat kst_; dentry *dentry_; uint64 off_; } en_;
			inline bool ensureDev() const { return  dv_.type_ == FT_DEVICE  ||
													dv_.type_ == FT_NONE ; };
			inline bool ensureEntry() const { return en_.type_ == FT_NORMAL; };
			inline bool ensurePipe() const { return pp_.type_ == FT_PIPE; };
		public:
			Data( FileTypes type ) 	: 	dv_( { type, type } ) { ensureDev(); }
			Data( dentry *de_ ) 	: 	en_( { FT_NORMAL, de_, de_, 0 } ) { ensureEntry(); }
			Data( Pipe *pipe_ ) 	: 	pp_( { FT_PIPE, pipe_, pipe_ } ) { ensurePipe(); }
			~Data() = default;
			inline dentry* get_Entry() const { ensureEntry(); return en_.dentry_; }
			inline Kstat & get_Kstat()
			{
				return en_.kst_;
			}
			inline uint64 & get_off()
			{
				ensureEntry();
					return en_.off_;
			}
			inline Pipe *get_Pipe()
			{
				ensurePipe();
				return pp_.pipe_;
			}
			inline FileTypes get_Type() const { return en_.type_; }
		} data;

		private:
			File() : refcnt( 0 ), data( FT_NONE ) {} // non-arg constructor only for file_p
		public:
			File( FileTypes type_ ) : refcnt( 0 ), data( type_ ) {}
			File( FileTypes type_, FileOps ops_ = FileOp::fileop_none ) : ops( ops_ ), refcnt( 0 ), data( type_ ) {}
			File( FileTypes type_, int flags_ ) : ops( flags_ ), refcnt( 0 ), data( type_ )  {}
			File( dentry *de_, int flags_ ) : flags( flags_ ), ops( flags_ ), refcnt( 0 ), data( de_ ) {}
			File( pm::ipc::Pipe *pipe, int flags_ ) : flags( flags_ ), ops( flags_ ), refcnt( 0 ), data( pipe ) {}
			~File() = default;

			int write( uint64 buf, size_t len ) { return 0; };
			int read( uint64 buf, size_t len, int off_ = 0, bool update = true ) { return 0; };

	};


	class xv6_file_pool;
	
	/// TODO: 这是搬运自xv6的file，在将来使用vfs后将弃用
	class xv6_file
	{
		friend xv6_file_pool;
	public:
		xv6_file() : ref( 0 ), readable( 0 ), writable( 0 ), dentry( nullptr ), off( 0 ), major( 0 ) {};
		
		enum { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
		int ref; // reference count
		char readable;
		char writable;
		fs::dentry * dentry;
		pm::ipc::Pipe *pipe; // FD_PIPE
		// struct inode *ip;  // FD_INODE and FD_DEVICE
		uint off;          // FD_INODE
		short major;       // FD_DEVICE
		fs::Kstat kst;
		
		int write( uint64 addr, int n );
		int read( uint64 addr, int n );

	};

	constexpr uint file_pool_max_size = 100;
	constexpr uint pipe_pool_max_size = 10;
	class file_pool
	{
	private:
		hsai::SpinLock _lock;
		File _files[ file_pool_max_size ];
		eastl::vector <eastl::string> _unlink_list;
	public:
		void init();
		File * alloc_file();
		void free_file( File * f );
		void dup( File * f );
		File * find_file( eastl::string path);
		int unlink( eastl::string path );
		bool has_unlinked( eastl::string path) { return eastl::find( _unlink_list.begin(), _unlink_list.end(), path ) != _unlink_list.end();};
	};

	extern file_pool k_file_table;

	class file
	{
		public:
			FileAttrs _attrs;
			uint32 refcnt;
			Kstat _stat;
		public:
		 	file() = delete;
			file( FileAttrs attrs ) : _attrs( attrs ), refcnt( 0 ), _stat( _attrs.filetype ) { }
			virtual ~file() = default;
			virtual void free_file() { refcnt--; if( refcnt == 0 ) delete this; };
			virtual int read( uint64 buf, size_t len, int off = 0, bool upgrade = false ) = 0;
			virtual int write( uint64 buf, size_t len ) = 0;
			virtual void dup() { refcnt++; }; 
			int readlink( uint64 buf, size_t len );
			//virtual int readlink( uint64 buf, size_t len ) = 0;
	};

} // namespace fs
