//
// Copyright (c) 2016 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#pragma once

#include "const_string.h"
#include <initializer_list>
#include <map>
#include <memory>
#include <vector>
#include <mutex>


////////////////////////////////////////


namespace base
{

///
/// @brief Class env provides a wrapper around environment variables
///
/// The primary access will be against the singleton provided by @sa
/// base::env::global, but when creating sub-processes, it may be
/// desireable to duplicate, set, and launch with a particular
/// environment.
class env
{
public:
	std::string get( cstring var ) const;
	void set( cstring var, cstring val );

	void clear( void );
	void clear_except( cstring var );
	void clear_except( std::initializer_list<cstring> l );

	/// takes a snapshot of the current env state and uses that to
	/// construct a (cached) set of data that can be used to launch a
	/// process with.
	///
	/// Under Windows, where it wants a string of all, this char **
	/// should be derefenced when passed to CreateProcess (there will
	/// be 1 item in the list)
	char **launch_vars( void ) const;

	static env &global( void );
private:
	void clear_cache( void );

	mutable std::mutex _mutex;
	std::map<std::string,std::string> _env;
	mutable std::vector<std::string> _launch_vals;
	mutable std::unique_ptr<char *[]> _launch_store;
};

} // namespace base



