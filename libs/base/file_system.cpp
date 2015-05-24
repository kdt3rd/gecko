
#include "file_system.h"
#include <map>
#include <mutex>
#include <system_error>

namespace
{

std::mutex theMutex;

std::map<std::string,std::shared_ptr<base::file_system>> &
singleton( void )
{
	static std::map<std::string,std::shared_ptr<base::file_system>> theFileSystems;
	return theFileSystems;
}

} // empty namespace


////////////////////////////////////////


namespace base
{

////////////////////////////////////////

file_system::~file_system( void )
{
}


////////////////////////////////////////


uri
file_system::stat( const uri &path, struct stat *buf )
{
	lstat( path, buf );

	int depth = 0;
	uri curpath = path;
	std::shared_ptr<file_system> fs;
	while ( S_ISLNK( buf->st_mode ) )
	{
		if ( fs )
			curpath = fs->readlink( curpath, buf->st_size + 1 );
		else
			curpath = readlink( curpath, buf->st_size + 1 );
		fs = get( curpath );
		fs->lstat( curpath, buf );
		depth++;
		if ( depth > 20 )
			throw std::system_error( ELOOP, std::system_category(), path.pretty() );
	}

	return std::move( curpath );
}


////////////////////////////////////////


void
file_system::rmdir_all( const uri &path )
{
	struct stat buf;
	lstat( path, &buf );
	if ( S_ISDIR( buf.st_mode ) )
	{
		std::vector<uri> paths;
		{
			auto dir = readdir( path );
			while ( ++dir )
				paths.push_back( *dir );
		}
		for ( auto &p: paths )
			rmdir_all( p );
		rmdir( path );
	}
	else
		unlink( path );
}


////////////////////////////////////////


std::shared_ptr<file_system>
file_system::get( const uri &path )
{
	precondition( path, "invalid uri" );
	std::unique_lock<std::mutex> lk( theMutex );
	auto &fs = singleton();
	auto f = fs.find( path.scheme() );
	precondition( f != fs.end(), "no file system registered for scheme in path " + path.pretty() );
	return f->second;
}


////////////////////////////////////////


void
file_system::add( const std::string &sch, const std::shared_ptr<file_system> &fs )
{
	std::unique_lock<std::mutex> lk( theMutex );
	auto &thefs = singleton();
	auto f = thefs.find( sch );
	precondition( f == thefs.end() || (f->second == fs), "Multiple file systems registered for same scheme " + sch );

	thefs[sch] = fs;
}

////////////////////////////////////////

}

