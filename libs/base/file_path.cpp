
#include "file_path.h"

namespace base
{

////////////////////////////////////////

std::string remove_extension( const std::string &file )
{
	size_t off = file.find_last_of( '.' );
	if ( off < file.size() )
		return file.substr( 0, off );
	return file;
}

////////////////////////////////////////

std::string remove_extension( std::string &&file )
{
	size_t off = file.find_last_of( '.' );
	if ( off < file.size() )
		file.erase( off );
	return std::move( file );
}

////////////////////////////////////////

std::string basename( const std::string &file )
{
	size_t start = file.find_last_of( '/' );
	if ( start > file.size() )
		start = 0;
	else
		start = start + 1;

	size_t end = file.find_last_of( '.' );
	if ( end > file.size() )
		end = file.size();

	return file.substr( start, end - start );
}

////////////////////////////////////////

}

