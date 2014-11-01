
#pragma once

#include <string>
#include <iostream>
#include <sstream>

namespace base
{

////////////////////////////////////////

std::string trim( const std::string &str, const std::string &ws = " \t\n\r" );
std::string replace( std::string &&str, char c, const std::string &replacement );

inline std::string replace( const std::string &str, char c, const std::string &replacement )
{
	return replace( std::string( str ), c, replacement );
}

template<typename T>
std::string to_string( const T &t )
{
	std::stringstream tmp;
	tmp << t;
	return tmp.str();
}

////////////////////////////////////////

}

