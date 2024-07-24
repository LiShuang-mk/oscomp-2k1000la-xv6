//
// Created by Li Shuang ( pseudonym ) on 2024-05-19 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "fs/file_defs.hh"
#include "fs/kstat.hh"

#include "pm/ipc/pipe.hh"
namespace pm
{
	namespace ipc{

		class Pipe;

	}
}

using namespace pm::ipc;
namespace fs
{	
	class Dentry;
	class dentry;
	class File
	{
	public:
		FileTypes type;
		int flags;
		FileOps ops;

		union Data
		{
		private:
			struct { const FileTypes type_; Kstat kst_; } dv_;
			struct { const FileTypes type_; Kstat kst_; Pipe *pipe_; } pp_;
			struct { const FileTypes type_; Kstat kst_; dentry *dentry_; uint64 off_; } en_;
			inline bool ensureDev() const { return  dv_.type_ == FT_DEVICE  ||
													dv_.type_ == FT_NONE	||
													dv_.type_ == FT_STDERR	||
													dv_.type_ == FT_STDIN   ||
													dv_.type_ == FT_STDOUT  ; };
			inline bool ensureEntry() const { return en_.type_ == FT_ENTRY; };
			inline bool ensurePipe() const { return pp_.type_ == FT_PIPE; };
		public:
			Data( FileTypes type ) 	: 	dv_( { type, type } ) { ensureDev(); }
			Data( dentry *de_ ) 	: 	en_( { FT_ENTRY, de_, de_, 0 } ) { ensureEntry(); }
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

		File( FileTypes type_ ) : data( type_ ) {}
		File( FileTypes type_, FileOps ops_ = FileOp::fileop_none ) : ops( ops_ ), data( type_ ) {}
		File( FileTypes type_, int flags_ ) : ops( flags_ ), data( type_ ) {}
		File( dentry *de_, int flags_ ) : flags( flags_ ), ops( flags_ ), data( de_ ) {}
		File( pm::ipc::Pipe *pipe, int flags_ ) : flags( flags_ ), ops( flags_ ), data( pipe ) {}
		~File() = default;

		int write( uint64 buf, size_t len );
		int read( uint64 buf, size_t len, int off_ = -1, bool update = true );

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
	class xv6_file_pool
	{
	private:
		hsai::SpinLock _lock;
		xv6_file _files[ file_pool_max_size ];
		eastl::vector <eastl::string> _unlink_list;
		pm::ipc::Pipe _pipe[ pipe_pool_max_size ];
		int pipe_map[ pipe_pool_max_size ];
	public:
		void init();
		xv6_file * alloc_file();
		void free_file( xv6_file * f );
		void dup( xv6_file * f );
		xv6_file * find_file( eastl::string path);
		int unlink( eastl::string path );
		bool has_unlinked( eastl::string path) { return eastl::find( _unlink_list.begin(), _unlink_list.end(), path ) != _unlink_list.end();};
		pm::ipc::Pipe * alloc_pipe();
	};

	extern xv6_file_pool k_file_table;

} // namespace fs
