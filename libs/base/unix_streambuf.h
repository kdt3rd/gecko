//
// Copyright (c) 2015 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "streambuf.h"
#include "uri.h"
#include <string>


////////////////////////////////////////


namespace base
{

///
/// @brief Provides a wrapper around a standard unix file descriptor.
///
/// implements all the virtual functions of streambuf appropriate for a
/// file descriptor. Should work for sockets and pipes as well.
class unix_streambuf : public streambuf
{
public:
	typedef streambuf::char_type char_type;
	typedef streambuf::off_type off_type;
	typedef streambuf::int_type int_type;
	typedef streambuf::pos_type pos_type;
	typedef streambuf::traits_type traits_type;

	unix_streambuf( std::ios_base::openmode m,
					int fd, bool doDup,
					const std::string &path = std::string(),
					std::streamsize bufSz = 0 );
	unix_streambuf( std::ios_base::openmode m, const uri &uri,
					std::streamsize bufSz = 0 );
	unix_streambuf( std::ios_base::openmode m, const std::string &path,
					std::streamsize bufSz = 0 );
	unix_streambuf( std::ios_base::openmode m, std::string &&path,
					std::streamsize bufSz = 0 );
	unix_streambuf( unix_streambuf &&u );
	unix_streambuf &operator=( unix_streambuf &&u );
	virtual ~unix_streambuf( void );

	void swap( unix_streambuf &u );

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
	unix_streambuf( const unix_streambuf & ) = delete;
	unix_streambuf &operator=( const unix_streambuf & ) = delete;

	void initFD( std::ios_base::openmode m );

	int _fd = -1;
	std::string _path;
};

} // namespace base



