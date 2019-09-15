// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "statement.h"

#include <cstring>
#include <string>

extern "C"
{
    struct sqlite3;
}

namespace sqlite
{
////////////////////////////////////////

/// @brief SQLite database.
class database
{
public:
    database( std::string dbfile );
    ~database( void );

    const std::string &filename( void ) const { return _filename; }

    void exec( const char *sql ) const;

    void exec( const std::string &sql ) const { return exec( sql.c_str() ); }

    //	int64_t last_inserted_id( void ) const;

    statement prepare( const char *sql )
    {
        return statement( _db, sql, std::strlen( sql ) );
    }

    statement prepare( const std::string &sql )
    {
        return statement( _db, sql.c_str(), sql.size() );
    }

private:
    void check( int err, const char *msg = nullptr ) const;

    std::string _filename;
    sqlite3 *   _db = nullptr;
};

////////////////////////////////////////

} // namespace sqlite
