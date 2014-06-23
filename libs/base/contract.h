
#pragma once

#include <cassert>
#include <stdexcept>

namespace base
{

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

void print_exception( std::ostream &out, const std::exception &e, int level = 0 );

////////////////////////////////////////

}

////////////////////////////////////////

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

