
#pragma once

#include <string>
#include <map>
#include <net/tcp_socket.h>

namespace web
{

////////////////////////////////////////

class web_base
{
public:
	web_base( void );
	web_base( std::string v );
	virtual ~web_base( void );
	web_base( const web_base & ) = default;
	web_base( web_base && ) = default;
	web_base &operator=( const web_base & ) = delete;
	web_base &operator=( web_base && ) = delete;

	/// @brief HTTP version
	/// This should be "1.1"
	const std::string &version( void ) const
	{
		return _version;
	}

	/// @brief Get HTTP content
	const std::string &content( void ) const
	{
		return _content;
	}

	/// @brief Set HTTP content
	void set_content( std::string &&c )
	{
		_content = std::move( c );
	}

	/// @brief Add HTTP header name/value pair.
	void set_header( std::string n, std::string v )
	{
		_header[std::move( n )] = std::move( v );
	}

	/// @brief Get an HTTP header value
	std::string operator[]( const std::string &n )
	{
		return _header.at( n );
	}

	/// @brief The map of HTTP header values.
	const std::map<std::string,std::string> &header( void ) const
	{
		return _header;
	}

protected:
	std::string read_line( net::tcp_socket &socket );
	void read_content( net::tcp_socket &sockte );

	std::map<std::string,std::string> _header;
	std::string _content;
	std::string _version = "1.1";
};

////////////////////////////////////////

}

