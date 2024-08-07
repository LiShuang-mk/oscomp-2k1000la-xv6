#include "fs/file/file.hh"

struct termios;

namespace hsai
{
	class StreamDevice;
} // namespace hsai


namespace fs
{
	class device_file : public file
	{
	private:
		int off = 0;
		hsai::StreamDevice * _dev = nullptr;

	public:
		device_file( FileAttrs attrs, uint dev );
		device_file( uint dev ) : device_file( FileAttrs( FileTypes::FT_DEVICE, 0777 ), dev ) {};
		~device_file() = default;

		long read( uint64 buf, size_t len, long off, bool upgrade = true ) override;
		long write( uint64 buf, size_t len, long off, bool upgrade = true ) override;

		virtual bool read_ready() override;
		virtual bool write_ready() override;

		int tcgetattr( termios * ts );
	};
}