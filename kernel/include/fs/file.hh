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

namespace fs
{
	class Dentry;

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
			//struct {const FileTypes type_; Pipe *pipe_} pp_;
			struct { const FileTypes type_; Kstat kst_; Dentry *dentry_; uint64 off_; } en_;
			inline bool ensureDev() const { return dv_.type_ == FT_NONE || dv_.type_ == FT_STDIN || dv_.type_ == FT_STDOUT || dv_.type_ == FT_DEVICE; };
			inline bool ensureEntry() const { return en_.type_ == FT_ENTRY; };
		public:
			Data( FileTypes type ) : dv_( { type, type } ) { ensureDev(); }
			Data( Dentry *de_ ) : en_( { FT_ENTRY,de_, de_, 0 } ) { ensureEntry(); }
			~Data() = default;
			inline Dentry* get_Entry() const { ensureEntry(); return en_.dentry_; }
			inline Kstat & get_Kstat()
			{ //if(ensureEntry()) 
				return en_.kst_;
			}
			inline uint64 & get_off()
			{ //if(ensureEntry()) 
				return en_.off_;
			}
			inline FileTypes get_Type() const { return en_.type_; }
		} data;

		File( FileTypes type_ ) : data( type_ ) {}
		File( FileTypes type_, FileOps ops_ = FileOp::fileop_none ) : ops( ops_ ), data( type_ ) {}
		File( FileTypes type_, int flags_ ) : ops( flags_ ), data( type_ ) {}
		File( Dentry *de_, int flags_ ) : flags( flags_ ), ops( flags_ ), data( de_ ) {}
		~File() = default;

		int write( void *buf, size_t len );
		int read( void *buf, size_t len, int off_ = -1, bool update = true );

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
		Dentry * dentry;
		// struct pipe *pipe; // FD_PIPE
		// struct inode *ip;  // FD_INODE and FD_DEVICE
		uint off;          // FD_INODE
		short major;       // FD_DEVICE
		fs::Kstat kst;
		
		int write( uint64 addr, int n );
		int read( uint64 addr, int n );

	};

	constexpr uint file_pool_max_size = 100;
	class xv6_file_pool
	{
	private:
		smp::Lock _lock;
		xv6_file _files[ file_pool_max_size ];
		eastl::vector <eastl::string> _unlink_list;
	public:
		void init();
		xv6_file * alloc_file();
		void free_file( xv6_file * f );
		void dup( xv6_file * f );
		xv6_file * find_file( eastl::string path);
		int unlink( eastl::string path );
		bool has_unlinked( eastl::string path) { return eastl::find( _unlink_list.begin(), _unlink_list.end(), path ) != _unlink_list.end();};
	};

	extern xv6_file_pool k_file_table;


} // namespace fs
