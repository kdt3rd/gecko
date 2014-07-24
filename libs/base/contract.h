
#pragma once

#include <cassert>
#include <stdexcept>
#include "format.h"

#define throw_location( exc ) \
	do { \
	try { throw base::exception_location( __FILE__, __LINE__ ); } \
	catch ( ... ) { std::throw_with_nested( exc ); } \
	} while ( false )

#define throw_add( ... ) \
		std::throw_with_nested( std::runtime_error( base::format( __VA_ARGS__ ) ) );

#define throw_runtime( ... ) \
		throw_location( std::runtime_error( base::format( __VA_ARGS__ ) ) )

#define precondition( check, ... ) \
	if ( !(check) ) throw_location( base::precondition_error( base::format( __VA_ARGS__ ) ) )

#define postcondition( check, ... ) \
	if ( !(check) ) throw_location( base::postcondition_error( base::format( __VA_ARGS__ ) ) )

namespace base
{

////////////////////////////////////////

class exception_location : public std::exception
{
public:
	exception_location( const char *file, int line )
	{
		std::ostringstream str;
		str << "file " << file << " line " << line;
		_msg = std::move( str.str() );
	}

	const char *file( void ) const
	{
		return _file;
	}

	int line( void ) const
	{
		return _line;
	}

	virtual const char *what( void ) const throw() override
	{
		return _msg.c_str();
	}

private:
	int _line;
	const char *_file;
	std::string _msg;
};

////////////////////////////////////////

void print_exception( std::ostream &out, const std::exception &e, int level = 0 );

////////////////////////////////////////

class precondition_error : public std::logic_error
{
public:
	using std::logic_error::logic_error;
};

////////////////////////////////////////

class postcondition_error : public std::logic_error
{
public:
	using std::logic_error::logic_error;
};

////////////////////////////////////////

}

////////////////////////////////////////

/*
template <typename T>
inline void precondition( const T &check, const std::string &msg )
{
	if ( !check )
		throw base::precondition_error( msg );
}

////////////////////////////////////////

template <typename T>
inline void postcondition( const T &check, const std::string &msg )
{
	if ( !check )
		throw base::postcondition_error( msg );
}
*/

////////////////////////////////////////

