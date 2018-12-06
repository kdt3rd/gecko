//
// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT
//

#include "user_info.h"
#include "contract.h"
#ifndef _WIN32
# include <unistd.h>
#endif
#include <memory>

////////////////////////////////////////

namespace
{
#ifndef _WIN32
static void fill_info( const base::cstring &usr, uid_t &id, gid_t &dgid, std::string &user, std::string &disp, std::string &home )
{
	long pbufsc = sysconf( _SC_GETPW_R_SIZE_MAX );
	size_t pbufsz = 1024;
	if ( pbufsc > 0 )
		pbufsz = static_cast<size_t>( pbufsc );

	std::unique_ptr<char[]> tmpbuf( new char[pbufsz] );
	do
	{
		struct passwd pwbuf;
		struct passwd *result = nullptr;
		int err;
		if ( usr.empty() )
			err = getpwuid_r( id, &pwbuf, tmpbuf.get(), pbufsz, &result );
		else
			err = getpwnam_r( usr.c_str(), &pwbuf, tmpbuf.get(), pbufsz, &result );

		if ( err == 0 )
		{
			if ( ! result )
				throw_runtime( "No user record for '{0}' found", usr );

			id = pwbuf.pw_uid;
			dgid = pwbuf.pw_gid;
			user = pwbuf.pw_name;
			disp = pwbuf.pw_gecos;
			home = pwbuf.pw_dir;
			break;
		}

		if ( err == ERANGE )
		{
			pbufsz *= 2;
			tmpbuf.reset( new char[pbufsz] );
			continue;
		}

		if ( err == EINTR )
			continue;

		throw_location( std::system_error( err, std::system_category(), base::format( "Unhandled error searching for user '{0}'", usr ) ) );
	} while ( false );
}
#endif
}

////////////////////////////////////////

namespace base
{

////////////////////////////////////////

user_info::user_info( void )
#ifndef _WIN32
	: _id( geteuid() )
#endif
{
#ifndef _WIN32
	fill_info( cstring(), _id, _def_gid, _user, _display, _home );
#endif
}

////////////////////////////////////////

#ifndef _WIN32
user_info::user_info( uid_t id )
	: _id( id )
{
	fill_info( cstring(), _id, _def_gid, _user, _display, _home );
}
#endif

////////////////////////////////////////

user_info::user_info( cstring usr )
{
#ifndef _WIN32
	if ( usr.empty() )
	{
		_id = geteuid();
		fill_info( usr, _id, _def_gid, _user, _display, _home );
	}
	else
		fill_info( usr, _id, _def_gid, _user, _display, _home );
#endif
}

////////////////////////////////////////

user_info::~user_info( void )
{
}

////////////////////////////////////////

} // base



