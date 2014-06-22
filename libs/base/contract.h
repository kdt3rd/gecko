
#pragma once

#include <cassert>
#include <stdexcept>

////////////////////////////////////////

class precondition_error : public std::runtime_error
{
	using std::runtime_error::runtime_error;
};

////////////////////////////////////////

class postcondition_error : public std::runtime_error
{
	using std::runtime_error::runtime_error;
};

////////////////////////////////////////

template <typename T>
inline void precondition( const T &check, const std::string &msg )
{
	if ( !check )
		throw precondition_error( msg );
}

////////////////////////////////////////

template <typename T>
inline void postcondition( const T &check, const std::string &msg )
{
	if ( !check )
		throw postcondition_error( msg );
}

////////////////////////////////////////

void print_exception( std::ostream &out, const std::exception &e, int level = 0 );

////////////////////////////////////////

