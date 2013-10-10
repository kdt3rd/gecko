
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

inline void precondition( bool check, const std::string &msg )
{
	if ( !check )
		throw precondition_error( msg );
}

////////////////////////////////////////

inline void postcondition( bool check, const std::string &msg )
{
	if ( !check )
		throw postcondition_error( msg );
}

////////////////////////////////////////

void print_exception( std::ostream &out, const std::exception &e, int level = 0 );

////////////////////////////////////////

