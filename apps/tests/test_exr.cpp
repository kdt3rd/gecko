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
#include <media/image.h>
#include <media/data.h>
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
				std::cout << " layers: " << f->layer_count() << std::endl;
				for ( size_t l = 0, L = f->layer_count(); l != L; ++l )
				{
					const auto &curl = f->layers()[l];
					std::cout << "   layer " << l << ": '" << curl.name() << "' " << curl.view_count() << " views (default: '" << curl.default_view_name() << "')\n";
					for ( size_t v = 0, V = curl.view_count(); v != V; ++v )
					{
						const auto &curv = curl[v];
						std::cout << "      view " << v << ": " << curv.name() << '\n';
						auto img = static_cast< std::shared_ptr<media::image> >( curv );
						if ( img )
						{
							std::cout << "        image active_area: " << img->active_area() << '\n';
							std::cout << "                   planes: ";
							for ( size_t p = 0; p != img->size(); ++p )
							{
								if ( p > 0 )
									std::cout << ", ";
								std::cout << img->plane_name( p );
							}
							std::cout << '\n';
						}
						auto dt = static_cast< std::shared_ptr<media::data> >( curv );
						if ( dt )
						{
							std::cout << "        deep active_area: " << dt->active_area() << '\n';
							std::cout << "                  planes: ";
							for ( size_t p = 0; p != dt->size(); ++p )
							{
								if ( p > 0 )
									std::cout << ", ";
								std::cout << dt->plane_name( p );
							}
							std::cout << '\n';
						}
					}
				}
				std::cout << std::endl;
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
