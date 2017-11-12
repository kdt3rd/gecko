//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <cstdint>
#include <sys/types.h>

////////////////////////////////////////

namespace base
{

///
/// @brief Class pipe provides...
///
class pipe
{
public:
	/// @brief construct a pipe object
	///
	/// A pipe object has a read and a write file descriptor which are connected.
	/// This is a standard abstraction around a unix-style pipe
	///
	/// @param isPrivate when true, file descriptors will close across an exec call
	/// @param blocking when true, the read side will block until there is data
	pipe( bool isPrivate = false, bool blocking = true );
	pipe( const pipe & ) = delete;
	pipe &operator=( const pipe & ) = delete;
	pipe( pipe && );
	pipe &operator=( pipe && );
	~pipe( void );

	intptr_t readable( void ) const { return _p[0]; }
	intptr_t writable( void ) const { return _p[1]; }

	ssize_t read( void *d, size_t n );
	ssize_t write( const void *d, size_t n );

	void shutdownRead( void );
	void shutdownWrite( void );

private:
#ifdef _WIN32
	HANDLE _p[2];
#else
	int _p[2];
#endif
};

} // namespace base



