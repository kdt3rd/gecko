
#include <base/contract.h>
#include "database.h"
#include "sqlite3.h"

namespace sqlite
{

////////////////////////////////////////

database::database( std::string dbfile )
	: _filename( std::move( dbfile ) )
{
	check( sqlite3_open( _filename.c_str(), &_db ) );
}

////////////////////////////////////////

database::~database( void )
{
	sqlite3_close( _db );
	_db = nullptr;
	_filename.clear();
}

////////////////////////////////////////

void database::exec( const char *sql ) const
{
	check( sqlite3_exec( _db, sql, NULL, NULL, NULL ) );
}

////////////////////////////////////////

void database::check( int err ) const
{
	precondition( _db != nullptr, "null sqlite database" );
	if ( err != SQLITE_OK )
		throw std::runtime_error( sqlite3_errmsg( _db ) );
}

////////////////////////////////////////


}
