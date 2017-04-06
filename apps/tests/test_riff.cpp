//
// Copyright (c) 2015 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <base/cmd_line.h>
#include <base/scope_guard.h>
#include <media/riff/chunk.h>

#include <iostream>
#include <fstream>
#include <fstream>

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
		for ( auto &v: opt.values() )
		{
			std::ifstream file( v );
			media::riff::chunk riff( file );
			std::cout << v << " = " << riff.id() << std::endl;
			if ( riff.id() == "RIFF" )
			{
				media::riff::chunk more( riff.seek( file ) );
				std::cout << "  type: " << more.id() << " is " << more.size() << " bytes" << std::endl;
			}
		}
	}

	return 0;
}

}

int main( int argc, char *argv[] )
{
	try
	{
		return safemain( argc, argv);
	}
	catch ( const std::exception &e )
	{
		base::print_exception( std::cerr, e );
	}
}
