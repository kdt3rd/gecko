// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include "streambuf.h"
#include "uri.h"
#include <string>
#include <windows.h>

////////////////////////////////////////

namespace base
{

///
/// @brief Provides a wrapper around a standard win32 file handle.
///
/// implements all the virtual functions of streambuf appropriate for a
/// file handle. Should work for sockets and pipes as well.
class win32_streambuf : public streambuf
{
public:
	typedef streambuf::char_type char_type;
	typedef streambuf::off_type off_type;
	typedef streambuf::int_type int_type;
	typedef streambuf::pos_type pos_type;
	typedef streambuf::traits_type traits_type;

	win32_streambuf( std::ios_base::openmode m,
					 HANDLE f, bool doDup,
					 const std::string &path = std::string(),
					 std::streamsize bufSz = 0 );
	win32_streambuf( std::ios_base::openmode m, const uri &uri,
					 std::streamsize bufSz = 0 );
	win32_streambuf( std::ios_base::openmode m, const std::string &path,
					 std::streamsize bufSz = 0 );
	win32_streambuf( std::ios_base::openmode m, std::string &&path,
					 std::streamsize bufSz = 0 );
	win32_streambuf( win32_streambuf &&u );
	win32_streambuf &operator=( win32_streambuf &&u );
	virtual ~win32_streambuf( void );

	void swap( win32_streambuf &u );

protected:
	virtual bool is_open( void ) const override;
	virtual void close( void ) noexcept override;
	virtual off_type bytes_avail( void ) override;
	virtual off_type seek( off_type off, std::ios_base::seekdir dir ) override;
	virtual std::streamsize read( void *outBuf, size_t numBytes ) override;
	virtual std::streamsize write( const void *outBuf, size_t numBytes ) override;
	virtual std::streamsize writev( const void *outBuf1, size_t numBytes1,
									const void *outBuf2, size_t numBytes2 ) override;

private:
	win32_streambuf( const win32_streambuf & ) = delete;
	win32_streambuf &operator=( const win32_streambuf & ) = delete;

	void initFile( std::ios_base::openmode m );

	HANDLE _f = INVALID_HANDLE_VALUE;
	std::string _path;
};

} // namespace base
