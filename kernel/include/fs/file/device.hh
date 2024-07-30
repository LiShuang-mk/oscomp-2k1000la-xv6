#include "fs/file/file.hh"

struct termios;

namespace fs
{
    class device_file : public file
    {
        private:
            uint dev;
            int off = 0;
            
        public: 
            device_file( FileAttrs attrs_, uint dev_ ) : file(attrs_), dev(dev_) { dup(); }
            device_file( uint dev ) : file(FileAttrs( FileTypes::FT_DEVICE, 0777)), dev(dev) { dup(); }
            ~device_file()  = default;

            int read( uint64 buf, size_t len, int off = 0, bool upgrade = false ) override ;
			int write( uint64 buf, size_t len ) override;

			int tcgetattr( termios * ts );
	};
}