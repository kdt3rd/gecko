//
// Copyright (c) 2015-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <base/uri.h>
#include <base/scope_guard.h>
#include <base/contract.h>
#include <base/cmd_line.h>
#include <base/posix_file_system.h>
#include <media/reader.h>
#include <media/sample.h>
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
			base::uri u( v );
			if ( ! u )
				u.set_scheme( "file" );

			media::container c = media::reader::open( u );
			for ( auto &vt: c.video_tracks() )
			{
				std::cout << "track '" << vt->name() << "' frames " << vt->begin() << " - " << vt->end() << " @ rate " << vt->rate() << std::endl;

				media::sample s( vt->begin(), vt->rate() );

				auto f = s( vt );
				std::cout << " size: " << f->width() << " x " << f->height() << ", " << f->size() << " channels" << std::endl;
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
		return safemain( argc, argv );
	}
	catch ( const std::exception &e )
	{
		base::print_exception( std::cerr, e );
	}
}
