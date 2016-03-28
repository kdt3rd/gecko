
#include <base/uri.h>
#include <base/scope_guard.h>
#include <base/contract.h>
#include <base/cmd_line.h>
#include <base/posix_file_system.h>
#include <media/container.h>
#include <media/video_track.h>
#include <sstream>
#include <iostream>
#include <typeindex>

namespace
{

int safemain( int argc, char *argv[] )
{
	base::cmd_line options( argv[0],
		base::cmd_line::option(  0,  "",      "<file> ...",    base::cmd_line::args,     "List of EXR files to show", true )
	);

	auto errhandler = base::make_guard( [&]() { std::cerr << options << std::endl; } );
	options.parse( argc, argv );
	errhandler.dismiss();

	if ( auto &opt = options["<file> ..."] )
	{
		for ( auto &v: opt.values() )
		{
			media::container c = media::container::create( base::uri( "file", base::cstring(), v ) );
			auto t = std::dynamic_pointer_cast<media::video_track>( c.at( 0 ) );
			std::cout << "track '" << t->name() << "' frames " << t->begin() << " - " << t->end() << " @ rate " << t->rate() << std::endl;

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
