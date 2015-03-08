
#pragma once

#include <string>
#include <memory>
#include "blob.h"

extern "C" { struct sqlite3_stmt; }

namespace sqlite
{

class statement;

////////////////////////////////////////

class field
{
public:
	~field( void );

	std::string name( void ) const;

	size_t size( void ) const;

	bool is_null( void ) const;

	operator int64_t() const;
	operator double() const;
	operator std::string() const;
	operator blob() const;

private:
	friend class statement;
	field( sqlite3_stmt *stmt, int idx );

	sqlite3_stmt *_stmt;
	int _index;
};

////////////////////////////////////////

}
