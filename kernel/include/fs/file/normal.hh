#include "fs/file/file.hh"
#include "fs/dentry.hh"

namespace mm
{
	class UserspaceStream;
}

namespace fs
{
	class normal_file : public file
	{
	protected:
		dentry *_den;
	public:
		normal_file() = default;
		normal_file( FileAttrs attrs, dentry *den ) : file( attrs ), _den( den ) { dup(); new ( &_stat ) Kstat( den ); }
		normal_file( dentry *den ) : file( den->getNode()->rMode() ), _den( den ) { dup(); new ( &_stat ) Kstat( den ); }
		~normal_file() = default;

		/// @note off=-1 表示不指定偏移使用文件内部偏移量
		virtual long read( uint64 buf, size_t len, long off = -1, bool upgrade = true ) override;

		/// @note off=-1 表示不指定偏移使用文件内部偏移量
		virtual long write( uint64 buf, size_t len, long off = -1, bool upgrade = true ) override;
		virtual bool read_ready() override;
		virtual bool write_ready() override;
		virtual off_t lseek( off_t offset, int whence ) override;

		using ubuf = mm::UserspaceStream;
		size_t read_sub_dir( ubuf &dst );
		void setAppend();
		dentry *getDentry() { return _den; }
	};
}