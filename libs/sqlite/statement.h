// Copyright (c) 2015 Ian Godin
// SPDX-License-Identifier: MIT

#pragma once

#include "blob.h"
#include "field.h"

extern "C" { struct sqlite3; struct sqlite3_stmt; }

namespace sqlite
{

////////////////////////////////////////

class statement
{
public:
	statement( statement &&s );
	~statement( void );

	statement( const statement &s ) = delete;
	statement( void ) = delete;

	void reset( void );
	void clear( void );

	template<typename ...Data>
	void bind( Data... data )
	{
		bindit( 1, data... );
	}

	template<typename ...Data>
	void exec( Data... data )
	{
		bindit( 1, data... );
		exec();
	}

	void exec( void );
	bool step( void );

	size_t size( void ) const;

	field get( size_t f );

	template<typename T>
	T get( size_t f )
	{
		return static_cast<T>( get( f ) );
	}


private:
	friend class database;
	friend class field;
	statement( sqlite3 *db, const char *sql, size_t nsql );

	template<typename First, typename ...Data>
	void bindit( int i, First f, Data... d )
	{
		bindit( i, f );
		bindit( i+1, d... );
	}

	void bindit( int i, int x );
	void bindit( int i, int64_t x );
	void bindit( int i, const std::string &x );
	void bindit( int i, const char *x );
	void bindit( int i, const blob &b );

	void check( int err );

	sqlite3 *_db = nullptr;
	sqlite3_stmt *_stmt = nullptr;
};

////////////////////////////////////////

}

