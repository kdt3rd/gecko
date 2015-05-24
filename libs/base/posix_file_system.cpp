
#include "posix_file_system.h"
#include "contract.h"
#include "unix_streambuf.h"

#include <cstring>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stddef.h>
#include <fstream>
#include <memory>
#include <system_error>


////////////////////////////////////////


namespace base
{


////////////////////////////////////////


void
posix_file_system::lstat( const uri &path, struct stat *buf )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::string fpath = path.full_path();
	if ( ::lstat( fpath.c_str(), buf ) != 0 )
		throw_errno( "Retrieving file info for {0}", fpath );
}


////////////////////////////////////////


void
posix_file_system::statfs( const uri &path, struct statvfs *s )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::string fpath = path.full_path();
	if ( ::statvfs( fpath.c_str(), s ) != 0 )
		throw_errno( "Retrieving filesystem info for {0}", fpath );
}


////////////////////////////////////////


uri
posix_file_system::readlink( const uri &path, size_t sz )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::string fpath = path.full_path();

	if ( sz == 0 )
	{
		struct stat sbuf;
		if ( ::lstat( fpath.c_str(), &sbuf ) != 0 )
			throw std::system_error( errno, std::system_category(), fpath );

		sz = sbuf.st_size + 1;
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

		if ( r <= sz )
			break;
		sz *= 2;
	} while ( true );

	linkname[r] = '\0';
	fpath.clear();
	fpath.append( linkname.get() );
	return uri( uri::unescape( fpath ) );
}


////////////////////////////////////////


bool posix_file_system::access( const uri &path, int mode )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::string fpath = path.full_path();
	return ::access( fpath.c_str(), mode ) == 0;
}


////////////////////////////////////////


directory_iterator posix_file_system::readdir( const uri &path )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::string fpath = path.full_path();
	std::shared_ptr<DIR> dir( ::opendir( fpath.c_str() ), []( DIR *d ) { ::closedir( d ); } );
	if ( !dir )
		throw_errno( "opendir failed on {0}", fpath );

	size_t n = std::max( ::pathconf( fpath.c_str(), _PC_NAME_MAX), long(255) ) + 1;
	n += offsetof( struct dirent, d_name );
	std::shared_ptr<struct dirent> dir_ent( reinterpret_cast<dirent*>( new char[n] ), []( struct dirent *d ) { delete [] reinterpret_cast<char*>( d ); } );

	auto next_entry = [=]( void )
	{
		struct dirent *result = NULL;
		while ( ::readdir_r( dir.get(), dir_ent.get(), &result ) == 0 )
		{
			if ( result )
			{
				if ( strcmp( result->d_name, "." ) != 0 && strcmp( result->d_name, ".." ) != 0 )
					return uri( path, result->d_name );
			}
			else
				return uri();
		}
		throw_errno( "reading directory {0}", path );
	};

	return directory_iterator( next_entry );
}


////////////////////////////////////////


void
posix_file_system::mkdir( const uri &path, mode_t mode )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::string fpath = path.full_path();
	if ( ::mkdir( fpath.c_str(), mode ) == -1 )
	{
		if ( errno != EEXIST )
			throw_errno( "Making directory {0}", fpath );
	}
}


////////////////////////////////////////


void
posix_file_system::mkdir_all( const uri &path, mode_t mode )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::string curpath;
	for ( auto &p: path.path() )
	{
		curpath += '/';
		curpath += p;
		if ( ::mkdir( curpath.c_str(), mode ) == -1 )
		{
			if ( errno != EEXIST )
				throw_errno( "Making directory {0}", curpath );
		}
	}
}


////////////////////////////////////////


void
posix_file_system::rmdir( const uri &path )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::string fpath = path.full_path();
	if ( ::rmdir( fpath.c_str() ) == -1 )
	{
		if ( errno != ENOENT )
			throw_errno( "Removing directory {0}", fpath );
	}
}


////////////////////////////////////////


void
posix_file_system::unlink( const uri &path )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::string fpath = path.full_path();
	if ( ::unlink( fpath.c_str() ) == -1 )
	{
		if ( errno != ENOENT )
			throw_errno( "Unlinking {0}", fpath );
	}
}


////////////////////////////////////////


void
posix_file_system::symlink( const uri &curpath, const uri &newpath )
{
	precondition( newpath.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::string newfpath = newpath.full_path();
	std::stringstream pathbuf;
	if ( curpath.scheme() == "file" )
		pathbuf << curpath.full_path();
	else
		pathbuf << curpath;
	std::string curfpath = pathbuf.str();
	if ( ::symlink( curfpath.c_str(), newfpath.c_str() ) == -1 )
		throw_errno( "Creating symlink {0} to target {1}", newfpath, curfpath );
}


////////////////////////////////////////


void
posix_file_system::link( const uri &curpath, const uri &newpath )
{
	precondition( curpath.scheme() == "file", "posix_file_system expected \"file\" uri" );
	precondition( newpath.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::string curfpath = curpath.full_path();
	std::string newfpath = newpath.full_path();
	if ( ::link( curfpath.c_str(), newfpath.c_str() ) != 0 )
		throw_errno( "Creating hard link from {0} to {1}", curfpath, newfpath );
}


////////////////////////////////////////


void
posix_file_system::rename( const uri &oldpath, const uri &newpath )
{
	precondition( oldpath.scheme() == "file", "posix_file_system expected \"file\" uri" );
	precondition( newpath.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::string oldfpath = oldpath.full_path();
	std::string newfpath = newpath.full_path();
	if ( ::rename( oldfpath.c_str(), newfpath.c_str() ) != 0 )
		throw_errno( "Renaming {0} to {1}", oldfpath, newfpath );
}


////////////////////////////////////////


istream
posix_file_system::open_read( const base::uri &path, std::ios_base::openmode m )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::unique_ptr<unix_streambuf> sb( new unix_streambuf( m, path, page_size ) );
	istream ret( std::move( sb ) );
	ret.exceptions( std::ios_base::failbit );
	return std::move( ret );
}


////////////////////////////////////////


ostream
posix_file_system::open_write( const base::uri &path, std::ios_base::openmode m )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::unique_ptr<unix_streambuf> sb( new unix_streambuf( m, path, page_size ) );
	ostream ret( std::move( sb ) );
	ret.exceptions( std::ios_base::failbit );
	return std::move( ret );
}


////////////////////////////////////////


iostream
posix_file_system::open( const base::uri &path, std::ios_base::openmode m )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	std::unique_ptr<unix_streambuf> sb( new unix_streambuf( m, path, page_size ) );
	iostream ret( std::move( sb ) );
	ret.exceptions( std::ios_base::failbit );
	return std::move( ret );
}


////////////////////////////////////////

}

