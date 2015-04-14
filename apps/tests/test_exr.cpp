
#include <base/uri.h>
#include <base/scope_guard.h>
#include <base/contract.h>
#include <base/cmd_line.h>
#include <base/posix_file_system.h>
#include <media/exr_reader.h>
#include <media/video_track.h>
#include <sstream>
#include <iostream>
#include <typeindex>

namespace
{

int safemain( int argc, char *argv[] )
{
	base::cmd_line options( argv[0],
		base::cmd_line::option(  0,  "",      "<file> ...",    base::cmd_line::args,     "List of RIFF files to show", true )
	);

	auto errhandler = base::make_guard( [&]() { std::cerr << options << std::endl; } );
	options.parse( argc, argv );
	errhandler.dismiss();

	if ( auto &opt = options["<file> ..."] )
	{
		base::file_system::add( "file", std::make_shared<base::posix_file_system>() );

		for ( auto &v: opt.values() )
		{
			media::container c = media::exr_reader( base::uri( "file", "", v ) );
			auto t = std::dynamic_pointer_cast<media::video_track>( c.at( 0 ) );
			auto f = t->at( t->begin() );
		}
	}

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
