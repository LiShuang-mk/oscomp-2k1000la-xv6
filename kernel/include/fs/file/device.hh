#include "fs/file/file.hh"

struct termios;

namespace hsai
{
	class StreamDevice;
} // namespace hsai


namespace fs
{
	class dentry;
	class device_file : public file
	{
	private:
		int off = 0;
		hsai::StreamDevice * _dev = nullptr;
		dentry * _dentry = nullptr;

	public:
		device_file( FileAttrs attrs, uint dev, dentry *den ) : file( attrs), _dentry( den ) { dup(); };  // 这里 device 的 dev已经没有用了，应该去node里面找
		device_file( uint dev, dentry *den ) : device_file( FileAttrs( FileTypes::FT_DEVICE, 0777 ), dev, den ) { dup();};
		~device_file() = default;

		long read( uint64 buf, size_t len, long off, bool upgrade = true ) override;
		long write( uint64 buf, size_t len, long off, bool upgrade = true ) override;

		virtual bool read_ready() override;
		virtual bool write_ready() override;
		virtual off_t lseek( off_t offset, int whence ) override { log_error( "streamdevice not support lseek currently!" );return -EINVAL; };
		int tcgetattr( termios * ts );
	};
}