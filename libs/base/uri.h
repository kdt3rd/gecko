
#pragma once

#include <string>
#include <iostream>
#include <cstdint>
#include <vector>


namespace base
{

////////////////////////////////////////

// scheme://user@host:port/path?query#fragment
class uri
{
public:
	uri( const std::string &str );

	const std::string &scheme( void ) const
	{
		return _scheme;
	}

	const std::string &user( void ) const
	{
		return _user;
	}

	const std::string &host( void ) const
	{
		return _host;
	}

	uint16_t port( void ) const
	{
		return _port;
	}

	const std::vector<std::string> &path( void ) const
	{
		return _path;
	}

	const std::string &query( void ) const
	{
		return _query;
	}

	const std::string &fragment( void ) const
	{
		return _fragment;
	}

	uri &operator=( const std::string &str )
	{
		parse( str );
		return *this;
	}

	void split_query( std::vector<std::pair<std::string,std::string>> &q );

	static std::string escape( const std::string &str );
	static std::string unescape( const std::string &str );

private:
	void parse( const std::string &str );

	std::string _scheme;
	std::string _user;
	std::string _host;
	uint16_t _port = 0;
	std::vector<std::string> _path;
	std::string _query;
	std::string _fragment;
};

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const uri &u );

////////////////////////////////////////

}

