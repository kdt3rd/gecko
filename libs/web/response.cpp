
#include "response.h"

namespace
{
	std::string read_line( net::tcp_socket &socket )
	{
		char c = '\0';
		socket.read( &c, 1 );

		std::string result;
		while ( c != '\r' )
		{
			result.push_back( c );
			socket.read( &c, 1 );
		}
		socket.read( &c, 1 );
		if ( c != '\n' )
			throw_runtime( "invalid HTTP line" );
		return std::move( result );
	}
}

namespace web
{

////////////////////////////////////////

response::response( net::tcp_socket &socket )
{
	std::string line = read_line( socket );

	size_t off = line.find( '/' );
	size_t off2 = line.find( ' ', off );
	size_t off3 = line.find( ' ', off2 + 1 );

	_version = line.substr( off + 1, off2 - off - 1 );
	_status = static_cast<status_code>( std::stoi( line.substr( off2 + 1, off2 - off3 - 1 ), nullptr, 10 ) );
	_reason = line.substr( off3 + 1 );

	line = read_line( socket );
	while ( !line.empty() )
	{
		size_t off = line.find( ':' );
		std::string key( line.substr( 0, off ) );
		std::string value( line.substr( off + 2 ) );
		_header[key] = value;
		line = read_line( socket );
	}

	auto te = _header.find( "Transfer-Encoding" );
	auto cl = _header.find( "Content-Length" );
	if ( te != _header.end() )
	{
		if ( te->second == "chunked" )
		{
			int size = 0;
			do
			{
				line = read_line( socket );
				size_t pos = 0;
				size = std::stoi( line, &pos, 16 );
				if ( size > 0 )
				{
					size_t off = _content.size();
					_content.resize( _content.size() + size );
					socket.read( &_content[off], size );
					char c[2];
					socket.read( &c, 2 );
					if ( c[0] != '\r' || c[1] != '\n' )
						throw_runtime( "invalid HTTP chunk" );
				}
			} while ( size > 0 );

		}
		else
			throw_runtime( "unknown Transfer-Encoding: {0}", te->second );
		/// @todo Read trailer headers
	}
	else if ( cl != _header.end() )
	{
		int size = std::stoi( cl->second, nullptr, 10 );
		size_t off = _content.size();
		_content.resize( _content.size() + size );
		socket.read( &_content[off], size );
	}
}

////////////////////////////////////////

void response::send( net::tcp_socket &socket )
{
	std::string tmp = base::format( "HTTP/{0} {1} {2}\r\n", _version, static_cast<int>( _status ), _reason );
	for ( auto &h: _header )
		tmp += base::format( "{0}: {1}\r\n", h.first, h.second );
	tmp += base::format( "Content-Length: {0}\r\n\r\n", _content.size() );
	socket.write( tmp.c_str(), tmp.size() );
	socket.write( _content.c_str(), _content.size() );
}

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const response &r )
{
	std::string tmp = base::format( "HTTP/{0} {1} {2}\r\n", r.version(), static_cast<int>( r.status() ), r.reason() );
	for ( auto &h: r.header() )
		tmp += base::format( "{0}: {1}\r\n", h.first, h.second );
	tmp += base::format( "Content-Length: {0}\r\n\r\n", r.content().size() );
	out << tmp;
	return out;
}

////////////////////////////////////////

}

