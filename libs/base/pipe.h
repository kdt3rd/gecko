//
// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT
// Copyrights licenced under the MIT License.
//

#pragma once

#include <cstdint>
#include <sys/types.h>
#include "wait.h"

////////////////////////////////////////

namespace base
{

///
/// @brief Class pipe provides...
///
class pipe
{
public:
	using handle = wait::wait_type;

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

	// TODO: convert intptr_t to a central waitable type
	// in base such that this can be inlined and just return
	// the native type so casts aren't needed everywhere...
	wait readable( void ) const { return wait( _p[0] ); }
	wait writable( void ) const { return wait( _p[1] ); }

	ssize_t read( void *d, size_t n );
	ssize_t write( const void *d, size_t n );

	void shutdownRead( void );
	void shutdownWrite( void );

private:
	handle _p[2];
};

} // namespace base



