//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "env.h"
#include <mutex>
#ifdef _WIN32
# include <windows.h>
#else
extern "C" char **environ;
#endif
#include "scope_guard.h"


////////////////////////////////////////


namespace
{

std::once_flag theEnvInitFlag;
void initGlobalEnv( base::env &e )
{
#ifdef _WIN32
	LPTCH envP = GetEnvironmentStrings();
	if ( ! envP )
		return;
	on_scope_exit{ FreeEnvironmentStrings( envP ); };
	std::string var, val;
	LPTCH curP = envP;
	while ( *curP != TCHAR(0) )
	{
		var.clear();
		val.clear();
		while ( *curP != TCHAR('=') )
		{
			var.push_back( *curP );
			++curP;
		}
		++curP;
		while ( *curP != TCHAR(0) )
		{
			val.push_back( *curP );
			++curP;
		}
		++curP;
		e.set( var, val );
	}
#else
	for ( size_t i = 0; environ[i]; ++i )
	{
		base::cstring t = environ[i];
		auto p = t.find_first_of( '=' );
		if ( p != base::cstring::npos )
			e.set( t.substr( 0, p ), t.substr( p + 1 ) );
	}
#endif
}

}


////////////////////////////////////////


namespace base
{

////////////////////////////////////////

std::string
env::get( cstring var ) const
{
	std::unique_lock<std::mutex> lk( _mutex );
	auto mi = _env.find( var );
	if ( mi == _env.end() )
		return std::string();
	return mi->second;
}

////////////////////////////////////////

bool
env::is_set( cstring var ) const
{
	std::unique_lock<std::mutex> lk( _mutex );
	auto mi = _env.find( var );
	return ( mi != _env.end() );
}

////////////////////////////////////////

void
env::set( cstring var, cstring val )
{
	std::unique_lock<std::mutex> lk( _mutex );
	_env[var] = val;
	clear_cache();
}

////////////////////////////////////////

void
env::clear( void )
{
	std::unique_lock<std::mutex> lk( _mutex );
	_env.clear();
	clear_cache();
}

////////////////////////////////////////

void
env::clear_except( cstring var )
{
	std::unique_lock<std::mutex> lk( _mutex );
	std::map<std::string,std::string> n;
	for ( auto &i: _env )
	{
		if ( var == i.first )
		{
			n[i.first] = i.second;
			break;
		}
	}
	std::swap( _env, n );
	clear_cache();
}

////////////////////////////////////////

void
env::clear_except( std::initializer_list<cstring> vars )
{
	std::unique_lock<std::mutex> lk( _mutex );
	std::map<std::string,std::string> n;
	for ( auto &i: _env )
	{
		for ( auto &sv: vars )
		{
			if ( sv == i.first )
			{
				n[i.first] = i.second;
				break;
			}
		}
	}
	std::swap( _env, n );
	clear_cache();
}

////////////////////////////////////////

char **
env::launch_vars( void ) const
{
	std::unique_lock<std::mutex> lk( _mutex );
	if ( _launch_store.get() != nullptr )
		return _launch_store.get();

#ifdef _WIN32
	size_t byteCount = 0;
	std::string accumString;
	for ( auto &i: _env )
	{
		accumString.append( i.first );
		accumString.push_back( '=' );
		accumString.append( i.second );
		accumString.push_back( '\0' );
	}
	accumString.push_back( '\0' );
	_launch_vals.emplace_back( std::move( accumString ) );
#else
	for ( auto &i: _env )
	{
		std::string tmp = i.first;
		tmp.push_back( '=' );
		tmp.append( i.second );
		_launch_vals.emplace_back( std::move( tmp ) );
	}
#endif
	_launch_store.reset( new char *[_launch_vals.size() + 1] );
	size_t i = 0;
	char **ptr = _launch_store.get();
	for ( ; i < _launch_vals.size(); ++i )
		ptr[i] = &(_launch_vals[i][0]);
	ptr[i] = nullptr;
	return ptr;
}

////////////////////////////////////////

env &
env::global( void )
{
	static env _glob;
	std::call_once( theEnvInitFlag, initGlobalEnv, std::ref( _glob ) );
	return _glob;
}

////////////////////////////////////////

void
env::clear_cache( void )
{
	_launch_vals.clear();
	_launch_store.reset();
}

////////////////////////////////////////


} // base



