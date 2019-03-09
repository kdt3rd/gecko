// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "win32_file_system.h"
#include <windows.h>
#include "scope_guard.h"
#include "contract.h"

////////////////////////////////////////

namespace base
{

////////////////////////////////////////

uri
win32_file_system::current_path( void ) const
{
	TCHAR buf[1024];
	// NB: not thread safe
	DWORD len = GetCurrentDirectory( 1024, buf );
	if ( len > 1024 )
	{
		std::unique_ptr<TCHAR[]> tmp( new TCHAR[len + 1] );
		len = GetCurrentDirectory( len + 1, tmp.get() );
		if ( len == 0 )
			throw_lasterror( "Unable to retrieve current directory" );
		return uri( tmp.get() );
	}
	else if ( len == 0 )
		throw_lasterror( "Unable to retrieve current directory" );

	return uri( buf );
}

////////////////////////////////////////

void
win32_file_system::lstat( const uri &path, struct stat *buf )
{
	precondition( path.scheme() == "file", "win32_file_system expected \"file\" uri" );
	std::string fpath = path.full_path();
	throw_not_yet();
	//GetFileAttributesEx()?
}


////////////////////////////////////////


void
win32_file_system::statfs( const uri &path, struct statvfs *s )
{
	precondition( path.scheme() == "file", "win32_file_system expected \"file\" uri" );
	throw_not_yet();
	//GetDiskFreeSpaceEx()
}


////////////////////////////////////////


uri
win32_file_system::readlink( const uri &path, size_t sz )
{
	precondition( path.scheme() == "file", "win32_file_system expected \"file\" uri" );
	std::string fpath = path.full_path();

	throw_not_yet();
#if 0
	if ( sz == 0 )
	{
		struct stat sbuf;
		if ( ::lstat( fpath.c_str(), &sbuf ) != 0 )
			throw std::system_error( errno, std::system_category(), fpath );

		sz = static_cast<size_t>( sbuf.st_size ) + 1;
	}
	std::unique_ptr<char[]> linkname;
	ssize_t r;
	// things might have changed size between the stat and now
	do
	{
		linkname.reset( new char[sz + 1] );
		r = ::readlink( fpath.c_str(), linkname.get(), sz + 1 );
		if ( r == -1 )
			throw std::system_error( errno, std::system_category(), fpath );

		if ( static_cast<size_t>( r ) <= sz )
			break;
		sz *= 2;
	} while ( true );

	linkname[static_cast<size_t>(r)] = '\0';
	std::cout << "Before: " << fpath << std::endl;
	if ( linkname[0] == '/' )
	{
		fpath.clear();
		fpath.append( linkname.get() );
		return uri( uri::unescape( fpath ) );
	}
	else
	{
		uri tmp = path.parent();
		tmp /= linkname.get();
		return tmp;
	}
#endif
}


////////////////////////////////////////


bool win32_file_system::access( const uri &path, int mode )
{
	precondition( path.scheme() == "file", "win32_file_system expected \"file\" uri" );
	throw_not_yet();
//	std::string fpath = path.full_path();
//	return ::access( fpath.c_str(), mode ) == 0;
}


////////////////////////////////////////


directory_iterator win32_file_system::readdir( const uri &path )
{
	precondition( path.scheme() == "file", "win32_file_system expected \"file\" uri" );
	std::string fpath = path.full_path();
	throw_not_yet();
#if 0
	std::shared_ptr<DIR> dir( ::opendir( fpath.c_str() ), []( DIR *d ) { ::closedir( d ); } );
	if ( !dir )
		throw_errno( "opendir failed on {0}", fpath );

	// glibc deprecates readdir_r in 2.24, handle that
#if defined(__GNU_LIBRARY__) && ( __GLIBC__ > 2 || ( __GLIBC__ == 2 && __GLIBC_MINOR__ >= 24 ) ) 
	// glibc deprecates readdir_r as readdir is safe to call in a multi-threaded environment
	// apparently using an internal lock on the temporary buffers used to make system calls
	// (with different DIR)
	auto next_entry = [=]( void )
	{
		while ( true )
		{
			errno = 0;
			struct dirent *result = ::readdir( dir.get() );
			if ( result )
			{
				if ( result->d_name[0] == '.' )
				{
					if ( result->d_name[1] == '\0' ||
						 ( result->d_name[1] == '.' && result->d_name[2] == '\0' ) )
					{
						continue;
					}
				}
				return uri( path, result->d_name );
			}
			else if ( errno != 0 )
				throw_errno( "reading directory {0}", path );

			// errno == 0, no result -> end of stream
			break;
		}
		return uri();
	};
#else
	size_t n = static_cast<size_t>( std::max( fpathconf( dirfd( dir.get() ), _PC_NAME_MAX ), long(255) ) ) + 1;
	n += offsetof( struct dirent, d_name );
	std::shared_ptr<struct dirent> dir_ent( reinterpret_cast<dirent*>( new char[n] ), []( struct dirent *d ) { delete [] reinterpret_cast<char*>( d ); } );

	auto next_entry = [=]( void )
	{
		struct dirent *result = NULL;
		while ( ::readdir_r( dir.get(), dir_ent.get(), &result ) == 0 )
		{
			if ( result )
			{
				if ( result->d_name[0] == '.' )
				{
					if ( result->d_name[1] == '\0' ||
						 ( result->d_name[1] == '.' && result->d_name[2] == '\0' ) )
						continue;
				}
				
				return uri( path, result->d_name );
			}

			return uri();
		}
		throw_errno( "reading directory {0}", path );
	};
#endif
	return directory_iterator( next_entry );
#endif
}


////////////////////////////////////////


void
win32_file_system::mkdir( const uri &path, mode_t mode )
{
	precondition( path.scheme() == "file", "win32_file_system expected \"file\" uri" );
	std::string fpath = path.full_path();
	throw_not_yet();
#if 0
	if ( ::mkdir( fpath.c_str(), mode ) == -1 )
	{
		if ( errno != EEXIST )
			throw_errno( "making directory {0}", fpath );
		else
		{
			struct stat buf = {};
			this->stat( path, &buf );
			if ( ! S_ISDIR( buf.st_mode ) )
			{
				errno = ENOTDIR;
				throw_errno( "making directory {0}", fpath );
			}
		}
	}
#endif
}


////////////////////////////////////////


void
win32_file_system::mkdir_all( const uri &path, mode_t mode )
{
	precondition( path.scheme() == "file", "win32_file_system expected \"file\" uri, got {0}", path.scheme() );
	base::uri curpath( path.root() );
	throw_not_yet();
#if 0
	for ( auto &p: path.path() )
	{
		curpath /= p;
		if ( ::mkdir( curpath.full_path().c_str(), mode ) == -1 )
		{
			if ( errno != EEXIST )
				throw_errno( "making directory {0}", curpath );
			else
			{
				struct stat buf = {};
				this->stat( curpath, &buf );
				if ( ! S_ISDIR( buf.st_mode ) )
				{
					errno = ENOTDIR;
					throw_errno( "making directory {0}", curpath );
				}
			}
		}
	}
#endif
}


////////////////////////////////////////


void
win32_file_system::rmdir( const uri &path )
{
	precondition( path.scheme() == "file", "win32_file_system expected \"file\" uri" );
	std::string fpath = path.full_path();
	throw_not_yet();
#if 0
	if ( ::rmdir( fpath.c_str() ) == -1 )
	{
		if ( errno != ENOENT )
			throw_errno( "removing directory {0}", fpath );
	}
#endif
}


////////////////////////////////////////


void
win32_file_system::unlink( const uri &path )
{
	precondition( path.scheme() == "file", "win32_file_system expected \"file\" uri" );
	std::string fpath = path.full_path();
	throw_not_yet();
#if 0
	if ( ::unlink( fpath.c_str() ) == -1 )
	{
		if ( errno != ENOENT )
			throw_errno( "unlinking {0}", fpath );
	}
#endif
}


////////////////////////////////////////


void
win32_file_system::symlink( const uri &curpath, const uri &newpath )
{
	precondition( newpath.scheme() == "file", "win32_file_system expected \"file\" uri" );
	std::string newfpath = newpath.full_path();
	std::stringstream pathbuf;
	if ( curpath.scheme() == "file" )
		pathbuf << curpath.full_path();
	else
		pathbuf << curpath;
	std::string curfpath = pathbuf.str();
	throw_not_yet();
#if 0
	if ( ::symlink( curfpath.c_str(), newfpath.c_str() ) == -1 )
		throw_errno( "creating symlink {0} to {1}", newfpath, curfpath );
#endif
}


////////////////////////////////////////


void
win32_file_system::link( const uri &curpath, const uri &newpath )
{
	precondition( curpath.scheme() == "file", "win32_file_system expected \"file\" uri, got {0}", curpath.scheme() );
	precondition( newpath.scheme() == "file", "win32_file_system expected \"file\" uri, got {0}", newpath.scheme() );
	std::string curfpath = curpath.full_path();
	std::string newfpath = newpath.full_path();
	throw_not_yet();
#if 0
	if ( ::link( curfpath.c_str(), newfpath.c_str() ) != 0 )
		throw_errno( "creating hard link from {0} to {1}", curfpath, newfpath );
#endif
}


////////////////////////////////////////


void
win32_file_system::rename( const uri &oldpath, const uri &newpath )
{
	precondition( oldpath.scheme() == "file", "win32_file_system expected \"file\" uri" );
	precondition( newpath.scheme() == "file", "win32_file_system expected \"file\" uri" );
	std::string oldfpath = oldpath.full_path();
	std::string newfpath = newpath.full_path();
	throw_not_yet();
#if 0
	if ( ::rename( oldfpath.c_str(), newfpath.c_str() ) != 0 )
		throw_errno( "renaming {0} to {1}", oldfpath, newfpath );
#endif
}


////////////////////////////////////////


istream
win32_file_system::open_read( const base::uri &path, std::ios_base::openmode m )
{
	precondition( path.scheme() == "file", "win32_file_system expected \"file\" uri" );
	throw_not_yet();
#if 0
	std::unique_ptr<unix_streambuf> sb( new unix_streambuf( m, path, page_size ) );
	istream ret( std::move( sb ) );
	ret.exceptions( std::ios_base::failbit );
	return ret;
#endif
}


////////////////////////////////////////


ostream
win32_file_system::open_write( const base::uri &path, std::ios_base::openmode m )
{
	precondition( path.scheme() == "file", "win32_file_system expected \"file\" uri" );
	throw_not_yet();
#if 0
	std::unique_ptr<unix_streambuf> sb( new unix_streambuf( m, path, page_size ) );
	return ostream( std::move( sb ) );
#endif
}


////////////////////////////////////////


iostream
win32_file_system::open( const base::uri &path, std::ios_base::openmode m )
{
	precondition( path.scheme() == "file", "win32_file_system expected \"file\" uri" );
	throw_not_yet();
#if 0
	std::unique_ptr<unix_streambuf> sb( new unix_streambuf( m, path, page_size ) );
	return iostream( std::move( sb ) );
#endif
}


////////////////////////////////////////


fs_watch
win32_file_system::watch( const base::uri &path,
						  const fs_watch::event_handler &evtcb,
						  fs_event evt_mask, bool recursive )
{
	throw_not_yet();
#if 0
	auto fsw = getWatcher();
	fs_watch retval( fsw, evtcb );
	fsw->registerWatch( path, retval, evt_mask, recursive );
	return retval;
#endif
}

////////////////////////////////////////

} // base



