//
// Copyright (c) 2016-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <iostream>
#include <sstream>
#include <random>
#include <functional>
#include <memory>

#include <base/cmd_line.h>
#include <base/posix_file_system.h>
#include <media/reader.h>
#include <gui/application.h>
#include <gui/window.h>
#include <viewer/viewer.h>

//constexpr double padding = 12;
static std::shared_ptr<gui::application> app;

namespace {

////////////////////////////////////////

int safemain( int argc, char **argv )
{
	base::cmd_line options( argv[0],
		base::cmd_line::option(  0,  "", "<img>", base::cmd_line::arg<0,1>, "Image to show", false )
	);

	auto errhandler = base::make_guard( [&]() { std::cerr << options << std::endl; } );
	options.parse( argc, argv );
	errhandler.dismiss();

	app = std::make_shared<gui::application>();
	app->push();

	auto win = app->new_window();
	win->set_title( app->active_platform() );

	win->in_context( [&]( void )
	{
		using namespace gui;

		auto view = viewer();

		win->set_widget( view );

		if ( auto &opt = options["<img>"] )
		{
			base::uri inputU( opt.value() );
			if ( ! inputU )
				inputU.set_scheme( "file" );

			media::container c = media::reader::open( inputU );
			for ( size_t ci = 0; ci != c.video_tracks().size(); ++ci )
			{
				auto &vt = c.video_tracks()[ci];
				int64_t fs = vt->begin();
				int64_t fe = vt->end();

				std::cout << "Sequence " << ci << " is from frame " << fs << " to " << fe << std::endl;
				if ( ci == 0 )
				{
					view->add_video_track( vt );
					view->update_frame( media::sample( fs, vt->rate() ) );
				}
			}
		}
	} );

	win->show();
	int code = app->run();
	app->pop();
	app.reset();
	return code;
}

////////////////////////////////////////

}

////////////////////////////////////////

int main( int argc, char *argv[] )
{
	int ret = -1;
	try
	{
		ret = safemain( argc, argv );
	}
	catch ( std::exception &e )
	{
		base::print_exception( std::cerr, e );
	}
	return ret;
}

////////////////////////////////////////

