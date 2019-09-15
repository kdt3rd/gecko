// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "const_string.h"
#ifndef _WIN32
#    include <pwd.h>
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
    user_info( user_info && )      = default;
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
