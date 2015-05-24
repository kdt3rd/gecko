
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


////////////////////////////////////////


namespace base
{

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

istream
posix_file_system::open_read( const base::uri &path, std::ios_base::openmode m )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	istream ret( std::make_shared<unix_streambuf>( m, path ) );
	ret.exceptions( std::ios_base::failbit );
	return std::move( ret );
}

////////////////////////////////////////

ostream
posix_file_system::open_write( const base::uri &path, std::ios_base::openmode m )
{
	precondition( path.scheme() == "file", "posix_file_system expected \"file\" uri" );
	ostream ret( std::make_shared<unix_streambuf>( m, path ) );
	ret.exceptions( std::ios_base::failbit );
	return std::move( ret );
}

////////////////////////////////////////

}

