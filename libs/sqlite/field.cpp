// Copyright (c) 2015 Ian Godin
// SPDX-License-Identifier: MIT

#include "field.h"

#include "sqlite3.h"

namespace sqlite
{
////////////////////////////////////////

field::~field( void ) {}

////////////////////////////////////////

std::string field::name( void ) const
{
    return sqlite3_column_name( _stmt, _index );
}

////////////////////////////////////////

size_t field::size( void ) const
{
    return static_cast<size_t>( sqlite3_column_bytes( _stmt, _index ) );
}

////////////////////////////////////////

bool field::is_null( void ) const
{
    return sqlite3_column_type( _stmt, _index ) == SQLITE_NULL;
}

////////////////////////////////////////

field::operator int64_t() const
{
    return sqlite3_column_int64( _stmt, _index );
}

////////////////////////////////////////

field::operator double() const
{
    return sqlite3_column_double( _stmt, _index );
}

////////////////////////////////////////

field::operator std::string() const
{
    const unsigned char *data = sqlite3_column_text( _stmt, _index );
    return std::string( reinterpret_cast<const char *>( data ), size() );
}

////////////////////////////////////////

field::operator blob() const
{
    const void *data = sqlite3_column_blob( _stmt, _index );
    return blob( data, size() );
}

////////////////////////////////////////

field::field( sqlite3_stmt *stmt, int idx ) : _stmt( stmt ), _index( idx ) {}

////////////////////////////////////////

} // namespace sqlite
