
#pragma once

#include <string>
#include <iostream>
#include <sstream>

namespace base
{

////////////////////////////////////////

std::string trim( const std::string &str, const std::string &ws = " \t\n\r" );

////////////////////////////////////////

template<typename Inserter>
inline void
split( const std::string &str, char delim, Inserter insert )
{
	/// @todo Implement this in a more efficient way.
	std::stringstream ss( str );
	std::string item;
	while ( std::getline( ss, item, delim ) )
		insert = item;
}

////////////////////////////////////////

std::string replace( std::string &&str, char c, const std::string &replacement );

inline std::string replace( const std::string &str, char c, const std::string &replacement )
{
	return replace( std::string( str ), c, replacement );
}

////////////////////////////////////////

template<typename T>
std::string to_string( const T &t )
{
	std::stringstream tmp;
	tmp << t;
	return tmp.str();
}

////////////////////////////////////////

inline bool begins_with( const std::string &s, const std::string &start )
{
	return s.compare( 0, start.length(), start ) == 0;
}

////////////////////////////////////////

}

