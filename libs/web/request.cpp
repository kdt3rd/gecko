
#include "request.h"

namespace web
{

////////////////////////////////////////

request::request( net::tcp_socket &socket )
{
	throw_not_yet();
}

////////////////////////////////////////

request::request( std::string method, const base::uri &path, std::string version )
	: _method( std::move( method ) ), _path( path ), _version( std::move( version ) )
{
}

////////////////////////////////////////

void request::send( net::tcp_socket &server )
{
	std::string tmp = base::format("{0} {1} HTTP/{2}\r\n", _method, _path.full_path(), _version );
	for ( auto &h: _header )
		tmp += base::format( "{0}: {1}\r\n", h.first, h.second );
	tmp += "\r\n";
	std::cout << "REQUEST:\n" << tmp << std::endl;
	server.write( tmp.c_str(), tmp.size() );
}

////////////////////////////////////////

}

