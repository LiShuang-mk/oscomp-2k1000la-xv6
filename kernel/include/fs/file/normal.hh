#include "fs/file/file.hh"
#include "fs/dentry.hh"

namespace fs
{
    class normal_file : public file
    {
        protected:
            uint64 _off = 0;
            dentry *_den;
	public:
		normal_file() = default;
		normal_file( FileAttrs attrs, dentry *den ) : file( attrs ), _den( den ) { dup(); new ( &_stat ) Kstat( den ); }
            normal_file( dentry *den ) : file( den->getNode()->rMode() ), _den( den ) { dup(); new ( &_stat ) Kstat ( den ); }
            ~normal_file() = default;

            virtual int read( uint64 buf, size_t len, int off = 0, bool upgrade = false ) override;
            virtual int write( uint64 buf, size_t len ) override;
            dentry *getDentry() { return _den; }
    };
}