
#include <base/uri.h>
#include <base/contract.h>
#include <base/cmd_line.h>
#include <base/scope_guard.h>
#include <base/posix_file_system.h>
#include <web/server.h>
#include <web/socket.h>

#include <stdlib.h>
#include <limits.h>

namespace
{

int safemain( int argc, char *argv[] )
{
	// Parse command-line arguments
	base::cmd_line options( argv[0],
		base::cmd_line::option( 0,   "",     "<dir>",   base::cmd_line::arg<1>,   "Directory to serve up", false ),
		base::cmd_line::option( 'p', "port", "<port>",  base::cmd_line::arg<1>,   "Port number to use",    false )
	);

	options.add_help();
	try
	{
		options.parse( argc, argv );
	}
	catch ( std::exception &e )
	{
		std::cerr << options << std::endl;
		base::print_exception( std::cerr, e );
		return -1;
	}

	uint16_t port = 8080;
	if ( auto &opt = options["port"] )
		port = static_cast<uint16_t>( std::stoul( opt.value() ) );

	std::string master_dir = ".";
	if ( auto &opt = options["<dir>"] )
		master_dir = opt.value();

	// Resolve relative paths (the simple way)
	{
		char fullpath[PATH_MAX];
		char *p = realpath( master_dir.c_str(), fullpath );
		if ( p == nullptr )
			throw_errno( "realpath resolving {0}", master_dir );
		master_dir = p;
	}

	// Setup file system.
	base::file_system::add( "file", std::make_shared<base::posix_file_system>() );
	auto fs = base::file_system::get( std::string( "file" ) );

	// Create a web server
	web::server server( port, 10 );
	server.default_resource( "GET" ) = [&]( web::request &req, net::tcp_socket &client )
	{
		base::uri path( "file", "", master_dir, req.path().full_path() );

		if ( fs->exists( path ) )
		{
			std::cout << "Sending " << path << std::endl;
			base::istream in = fs->open_read( path );
			web::response resp;
			resp.send( client, in );
		}
		else
		{
			std::cout << "File " << path << " not found" << std::endl;
			server.not_found( req, client );
		}
	};

	std::cout << base::format( "Serving directory {0} on port {1}", master_dir, port ) << std::endl;

	server.run();

	return 0;
}

}

int main( int argc, char *argv[] )
{
	try
	{
		return safemain( argc, argv );
	}
	catch ( const std::exception &e )
	{
		base::print_exception( std::cerr, e );
	}
}
