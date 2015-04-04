
#pragma once

#include <string>
#include <iostream>
#include <cstdint>
#include <vector>
#include <map>
#include <memory>

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

	std::vector<std::string> &path( void )
	{
		return _path;
	}

	std::string full_path( void ) const
	{
		std::string result;
		for ( auto &p: _path )
		{
			result += '/';
			result += p;
		}
		if ( _path.empty() )
			result.push_back( '/' );
		return result;
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

	std::shared_ptr<std::istream> open_for_read( void );
	std::shared_ptr<std::ostream> open_for_write( void );

	static std::string escape( const std::string &str );
	static std::string unescape( const std::string &str );

	static void register_read_handler( const std::string &sch, const std::function<std::shared_ptr<std::istream>(const base::uri &)> &open );
	static void register_write_handler( const std::string &sch, const std::function<std::shared_ptr<std::ostream>(const base::uri &)> &open );

private:
	static std::map<std::string,std::function<std::shared_ptr<std::istream>(const base::uri &)>> _readers;
	static std::map<std::string,std::function<std::shared_ptr<std::ostream>(const base::uri &)>> _writers;

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

