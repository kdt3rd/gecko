
#pragma once

#include <base/uri.h>
#include <net/tcp_socket.h>
#include <map>

namespace web
{

////////////////////////////////////////

/// @brief HTTP request
class request
{
public:
	/// @brief Server constructor
	/// The request is read from the (client) socket.
	request( net::tcp_socket &socket );

	/// @brief Client constructor
	/// Construct a request to send to the server.
	request( std::string method, const base::uri &path, std::string version = "1.1" );

	/// @brief HTTP method
	/// The method (e.g GET, PUT, or POST).
	const std::string &method( void ) const
	{
		return _method;
	}

	/// @brief HTTP request URI
	const base::uri &path( void ) const
	{
		return _path;
	}

	/// @brief HTTP version
	/// This should be "1.1"
	const std::string &version( void ) const
	{
		return _version;
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

	/// @brief Get the full map of HTTP header.
	const std::map<std::string,std::string> &header( void ) const
	{
		return _header;
	}

	/// @brief Send the request to the give socket
	void send( net::tcp_socket &socket );

private:
	std::string _method;
	base::uri _path;
	std::string _version;
	std::map<std::string,std::string> _header;
	std::stringstream _content;
};

////////////////////////////////////////

/// @brief Print (for debugging)
std::ostream &operator<<( std::ostream &out, const request &r );

////////////////////////////////////////

}

