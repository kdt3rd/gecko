
#pragma once

#include <string>
#include <iostream>
#include <cstdint>
#include <vector>
#include "contract.h"

namespace base
{

////////////////////////////////////////

// scheme://user@host:port/path?query#fragment
class uri
{
public:
	uri( void )
	{
	}

	uri( const std::string &str )
	{
		parse( str );
	}

	template<typename ... Types>
	uri( std::string sch, std::string auth, Types ...paths )
	{
		_scheme = sch;
		parse_authority( auth );
		add_paths( paths... );
	}

	template<typename ... Types>
	uri( const uri &parent, Types ...paths )
		: uri( parent )
	{
		add_paths( paths... );
	}

	/// @brief Construct URI and hide the password if present.
	explicit uri( char *str );

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

	uint16_t port( uint16_t default_port ) const
	{
		if ( _port == 0 )
			return default_port;
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

	const std::string &path( size_t i ) const
	{
		return _path.at( i );
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

	uri &operator/=( const std::string &str )
	{
		add_path( str );
		return *this;
	}

	uri parent( void ) const
	{
		precondition( !_path.empty(), "no parent uri" );

		uri result( *this );
		result.path().pop_back();
		return result;
	}

	void add_path( const std::string &str );

	template<typename ...Types>
	void add_paths( const std::string &str, Types ...rest )
	{
		add_path( str );
		add_paths( rest... );
	}

	void add_paths( void )
	{
	}

	void split_query( std::vector<std::pair<std::string,std::string>> &q );

	std::string pretty( void ) const;

	explicit operator bool( void ) const
	{
		return !_scheme.empty();
	}

	static std::string escape( const std::string &str );
	static std::string unescape( const std::string &str );

private:
	void parse( const std::string &str );
	void parse_authority( const std::string &str );

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

inline uri operator/( const uri &u, const std::string &str )
{
	return std::move( uri( u ) /= str );
}

inline uri operator/( uri &&u, const std::string &str )
{
	return std::move( u /= str );
}

////////////////////////////////////////

}

