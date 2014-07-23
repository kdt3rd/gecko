
#pragma once

#include <cassert>
#include <stdexcept>
#include "format.h"

namespace base
{

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

////////////////////////////////////////

template <typename ... Args>
inline void runtime_error( const std::string &msg, const Args &...data )
{
	if ( std::current_exception() )
		std::throw_with_nested( std::runtime_error( base::format( msg, data... ) ) );
	else
		throw std::runtime_error( base::format( msg, data... ) );
}

////////////////////////////////////////
