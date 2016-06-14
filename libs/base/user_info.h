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
#ifndef _WIN32
# include <pwd.h>
#endif

////////////////////////////////////////

namespace base
{

///
/// @brief Class user_info provides...
///
class user_info
{
public:
	// default constructs with current user
	user_info( void );
#ifndef _WIN32
	explicit user_info( uid_t uid );
#endif
	explicit user_info( cstring usr );
	~user_info( void );
	user_info( const user_info & ) = default;
	user_info( user_info && ) = default;
	user_info &operator=( const user_info & ) = default;
	user_info &operator=( user_info && ) = default;

	inline const std::string &user_name( void ) const { return _user; }
	inline const std::string &display_name( void ) const { return _display; }
	inline const std::string &home_dir( void ) const { return _home; }

	/// windows has an SID, but it's not a simple number, what to do?
#ifndef _WIN32
	/// TBD: effective vs. real?
	inline uid_t user_id( void ) const { return _id; }
	inline gid_t default_group_id( void ) const { return _def_gid; }
#endif

private:
	std::string _user;
	std::string _display;
	std::string _home;
#ifndef _WIN32
	uid_t _id;
	gid_t _def_gid;
#endif
};

} // namespace base



