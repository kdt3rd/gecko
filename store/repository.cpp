
#include <unistd.h>
#include <sys/stat.h>
#include "repository.h"

////////////////////////////////////////

repository::repository( const char *repo )
	: _repo( repo )
{
}

////////////////////////////////////////

repository::~repository( void )
{
}

////////////////////////////////////////

data_reader repository::open( const hash256 &h )
{
	std::string filename = _repo;
	std::string hash = h.string();
	for ( size_t i = 0; i < hash.size(); i += 4 )
	{
		filename += '/';
		filename += hash.substr( i, 4 );
	}

	data_reader tmp( filename );
	return tmp;
}

////////////////////////////////////////

data_writer repository::create( void )
{
	std::string filename = _repo + "/temp.XXXXXX";
	int fh = mkstemp( const_cast<char *>( filename.c_str() ) );
	if ( fh < 0 )
		throw std::runtime_error( "can't open temp file" );
	data_writer writer( filename );
	::close( fh );

	return writer;
}

////////////////////////////////////////

hash256 repository::close( data_writer &writer )
{
	std::string filename = _repo;
	hash256 h = writer.close();
	std::string hash = h.string();
	for ( size_t i = 0; i < hash.size(); i += 4 )
	{
		filename += '/';
		filename += hash.substr( i, 4 );
		if ( i + 4 < hash.size() )
		{
			if ( ::mkdir( filename.c_str(), 0755 ) != 0 )
			{
				if ( errno != EEXIST )
					throw std::runtime_error( "can't make directory" );
			}
		}
	}

	rename( writer.filename().c_str(), filename.c_str() );

	return h;
}

////////////////////////////////////////

