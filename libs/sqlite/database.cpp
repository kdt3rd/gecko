// Copyright (c) 2015 Ian Godin
// SPDX-License-Identifier: MIT

#include "database.h"

#include "sqlite3.h"

#include <base/contract.h>

namespace sqlite
{
////////////////////////////////////////

database::database( std::string dbfile ) : _filename( std::move( dbfile ) )
{
    check( sqlite3_open( _filename.c_str(), &_db ), _filename.c_str() );
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
    check( sqlite3_exec( _db, sql, NULL, NULL, NULL ), sql );
}

////////////////////////////////////////

void database::check( int err, const char *msg ) const
{
    precondition( _db != nullptr, "null sqlite database" );
    if ( err != SQLITE_OK )
    {
        if ( msg )
            throw_runtime( "{0}:\n{1}", sqlite3_errmsg( _db ), msg );
        else
            throw std::runtime_error( sqlite3_errmsg( _db ) );
    }
}

////////////////////////////////////////

} // namespace sqlite
