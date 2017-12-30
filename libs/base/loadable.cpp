//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#ifdef _WIN32
# include <windows.h>
#else
# ifdef __linux__
#  ifndef _GNU_SOURCE
#   define _GNU_SOURCE
#  endif
# endif
# include <dlfcn.h>
# include <atomic>
#endif
#include "loadable.h"
#include "contract.h"

////////////////////////////////////////

namespace base
{

////////////////////////////////////////

loadable::loadable( std::string lpath, isolation iso )
	: _loadpath( std::move( lpath ) )
{
#ifdef _WIN32
	HMODULE x = LoadLibrary( static_cast<LPCTSTR>( _loadpath.c_str() ) );
	if ( x == NULL )
		throw_lasterror( "Unable to load '{0}'", lpath );
	_dso.reset( static_cast<void *>( x ), []( void *p ) { FreeLibrary( reinterpret_cast<HMODULE>(p) ); } );
#else
	// TODO: Add a flag for RTLD_NOW vs RTLD_LAZY?
	void *d = nullptr;
	if ( iso == isolation::ISOLATE )
	{
#ifdef __linux__
		static std::atomic<Lmid_t> h_namespace( LM_ID_NEWLM );
		Lmid_t ns = h_namespace.load();

		d = dlmopen( ns, _loadpath.c_str(), RTLD_LAZY | RTLD_LOCAL );
		if ( d )
		{
			if ( ns == LM_ID_NEWLM )
			{
				if ( dlinfo( d, RTLD_DI_LMID, &ns ) == -1 )
				{
					std::string err = dlerror();
					dlclose( d );
					throw_runtime( "Unable to query namespace id: {0}", err );
				}
				
				h_namespace.store( ns );
			}
		}
#else
		d = dlopen( _loadpath.c_str(), RTLD_LAZY | RTLD_LOCAL );
#endif
	}
	else if ( iso == isolation::GLOBALIZE )
		d = dlopen( _loadpath.c_str(), RTLD_LAZY | RTLD_GLOBAL );
	else if ( iso == isolation::NORMAL )
		d = dlopen( _loadpath.c_str(), RTLD_LAZY | RTLD_LOCAL );

	if ( ! d )
	{
		std::string err = dlerror();
		throw_runtime( "Unable to open DSO: {0}", err );
	}

	_dso.reset( d, dlclose );
#endif
}

////////////////////////////////////////

loadable::~loadable( void )
{
}

////////////////////////////////////////

void *
loadable::symbol( const const_string<char> &symName, const cstring &symVersion )
{
#ifdef _WIN32
	auto r = GetProcAddress( reinterpret_cast<HMODULE>( _dso.get() ), symName.c_str() );
	return reinterpret_cast<void *>( r );
#elif defined __linux__
	if ( symVersion.empty() )
		return dlsym( _dso.get(), symName.c_str() );
	return dlvsym( _dso.get(), symName.c_str(), symVersion.c_str() );
#else
	(void)symVersion;
	return dlsym( _dso.get(), symName.c_str() );
#endif
}

////////////////////////////////////////

void *
loadable::find_app_symbol( const cstring &symName, const cstring &symVersion )
{
#ifdef _WIN32
	return reinterpret_cast<void *>( GetProcAddress( GetModuleHandle( NULL ), symName.c_str() ) );
#elif defined __linux__
	if ( symVersion.empty() )
		return dlsym( RTLD_DEFAULT, symName.c_str() );
	return dlvsym( RTLD_DEFAULT, symName.c_str(), symVersion.c_str() );
#else
	(void)symVersion;
	return dlsym( RTLD_DEFAULT, symName.c_str() );
#endif
}

////////////////////////////////////////

void *
loadable::find_next_symbol( const cstring &symName, const cstring &symVersion )
{
#ifdef _WIN32
	return find_app_symbol( symName, symVersion );
#elif defined __linux__
	if ( symVersion.empty() )
		return dlsym( RTLD_NEXT, symName.c_str() );
	return dlvsym( RTLD_NEXT, symName.c_str(), symVersion.c_str() );
#else
	(void)symVersion;
	return dlsym( RTLD_NEXT, symName.c_str() );
#endif
}

////////////////////////////////////////

} // base



