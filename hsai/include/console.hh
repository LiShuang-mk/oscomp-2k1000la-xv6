//
// Created by Li Shuang ( pseudonym ) on 2024-07-25 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "stream_device.hh"
#include "char_device.hh"

namespace hsai
{
	class ConsoleStdin : public StreamDevice
	{
	public:
		virtual long write( void * src, long n_bytes ) override;
		virtual long read( void * dst, long n_bytes ) override;
		virtual bool read_ready() override
		{
			if ( _stream == nullptr )
				return false;
			return _stream->read_ready();
		}
		virtual bool write_ready() override { return false; }
	};

	class ConsoleStdout : public StreamDevice
	{
	public:
		virtual long write( void * src, long n_bytes ) override;
		virtual long read( void * dst, long n_bytes ) override;
		virtual bool write_ready() override
		{
			if ( _stream == nullptr )
				return false;
			return _stream->write_ready();
		}
		virtual bool read_ready() override { return false; }
	};

	class ConsoleStderr : public StreamDevice
	{
	public:
		virtual long write( void * src, long n_bytes ) override;
		virtual long read( void * dst, long n_bytes ) override;
		virtual bool write_ready() override
		{
			if ( _stream == nullptr )
				return false;
			return _stream->write_ready();
		}
		virtual bool read_ready() override { return false; }
	};

} // namespace hsai
