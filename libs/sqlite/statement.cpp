
#include <cstring>
#include <base/contract.h>
#include "statement.h"
#include "sqlite3.h"

namespace sqlite
{

////////////////////////////////////////

statement::statement( statement &&s )
	: _db( s._db ), _stmt( s._stmt )
{
}

////////////////////////////////////////

statement::~statement( void )
{
	sqlite3_finalize( _stmt );
	_stmt = nullptr;
}

////////////////////////////////////////

void statement::reset( void )
{
	int ret = sqlite3_reset( _stmt );
	if ( ret != SQLITE_OK )
		throw_runtime( sqlite3_errmsg( _db ) );
}

////////////////////////////////////////

void statement::clear( void )
{
	int ret = sqlite3_clear_bindings( _stmt );
	if ( ret != SQLITE_OK )
		throw_runtime( sqlite3_errmsg( _db ) );
}

////////////////////////////////////////

void statement::exec( void )
{
	if ( step() )
		throw_runtime( "expected sql with no data" );
	reset();
	clear();
}

////////////////////////////////////////

bool statement::step( void )
{
	int ret = sqlite3_step( _stmt );
	switch ( ret )
	{
		case SQLITE_ROW:
			return true;

		case SQLITE_DONE:
			return false;

		default:
			throw_runtime( sqlite3_errmsg( _db ) );
	}
}

////////////////////////////////////////

size_t statement::size( void ) const
{
	throw_not_yet();
}

////////////////////////////////////////

field statement::get( size_t f )
{
	return field( _stmt, static_cast<int>( f ) );
}

////////////////////////////////////////

statement::statement( sqlite3 *db, const char *sql, size_t nsql )
	: _db( db )
{
	int ret = sqlite3_prepare_v2( db, sql, static_cast<int>( nsql ), &_stmt, NULL );
	if ( ret != SQLITE_OK )
		throw_runtime( sqlite3_errmsg( _db ) );
}

////////////////////////////////////////

void statement::bindit( int i, int x )
{
	int ret = sqlite3_bind_int( _stmt, i, x );
	if ( ret != SQLITE_OK )
		throw_runtime( "{0} (field {1})", sqlite3_errmsg( _db ), i );
}

////////////////////////////////////////

void statement::bindit( int i, int64_t x )
{
	int ret = sqlite3_bind_int64( _stmt, i, x );
	if ( ret != SQLITE_OK )
		throw_runtime( "{0} (field {1})", sqlite3_errmsg( _db ), i );
}

////////////////////////////////////////

void statement::bindit( int i, const std::string &x )
{
	int ret = sqlite3_bind_text64( _stmt, i, x.c_str(), x.size(), SQLITE_TRANSIENT, SQLITE_UTF8 );
	if ( ret != SQLITE_OK )
		throw_runtime( "{0} (field {1})", sqlite3_errmsg( _db ), i );
}

////////////////////////////////////////

void statement::bindit( int i, const char *x )
{
	int ret =sqlite3_bind_text64( _stmt, i, x, std::strlen( x ), SQLITE_TRANSIENT, SQLITE_UTF8 );
	if ( ret != SQLITE_OK )
		throw_runtime( "{0} (field {1})", sqlite3_errmsg( _db ), i );
}

////////////////////////////////////////

void statement::bindit( int i, const blob &b )
{
	int ret = sqlite3_bind_blob64( _stmt, i, b.data(), b.size(), SQLITE_TRANSIENT );
	if ( ret != SQLITE_OK )
		throw_runtime( "{0} (field {1})", sqlite3_errmsg( _db ), i );
}

////////////////////////////////////////

}

