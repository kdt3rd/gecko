
#include "request.h"
#include <iostream>

namespace web
{

////////////////////////////////////////

request::request( net::tcp_socket &socket )
{
	std::string line = read_line( socket );
	size_t off = line.find( ' ' );
	size_t off2 = line.find( ' ', off + 1 );
	size_t off3 = line.find( '/', off2 + 1 );
	_method = line.substr( 0, off );
	std::string tmp_path = line.substr( off + 1, off2 - off - 1 );
	_version = line.substr( off3 + 1 );

	line = read_line( socket );
	while ( !line.empty() )
	{
		size_t off = line.find( ':' );
		std::string key( line.substr( 0, off ) );
		std::string value( line.substr( off + 2 ) );
		_header[key] = value;
		line = read_line( socket );
	}

	auto host = _header.find( "Host" );
	if ( host != _header.end() )
		_path = base::uri( "http", host->second, tmp_path );

	read_content( socket );
}

////////////////////////////////////////

request::request( std::string method, const base::uri &path, std::string version )
	: web_base( std::move( version ) ), _method( std::move( method ) ), _path( path )
{
}

////////////////////////////////////////

void request::send( net::tcp_socket &server )
{
	std::string tmp = base::format("{0} {1} HTTP/{2}\r\n", _method, _path.full_path(), _version );
	for ( auto &h: _header )
		tmp += base::format( "{0}: {1}\r\n", h.first, h.second );
	if ( !_content.empty() )
		tmp += base::format( "Content-Length: {0}\r\n", _content.size() );
	tmp += "\r\n";
	server.write( tmp.c_str(), tmp.size() );
	if ( !_content.empty() )
		server.write( _content.c_str(), _content.size() );
}

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const request &r )
{
	std::string tmp = base::format("{0} {1} HTTP/{2}\r\n", r.method(), r.path().full_path(), r.version() );
	for ( auto &h: r.header() )
		tmp += base::format( "{0}: {1}\r\n", h.first, h.second );
	tmp += "\r\n";
	out << tmp;
	return out;
}

////////////////////////////////////////

}

