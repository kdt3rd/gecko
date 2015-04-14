
#include <iostream>
#include <sstream>
#include <random>
#include <functional>
#include <memory>

#include <base/cmd_line.h>
#include <base/posix_file_system.h>
#include <media/exr_reader.h>
#include <media/video_track.h>
#include <gui/application.h>
#include <viewer/viewer.h>

//constexpr double padding = 12;
std::shared_ptr<gui::application> app;

namespace {

////////////////////////////////////////

int safemain( int argc, char **argv )
{
	base::cmd_line options( argv[0],
		base::cmd_line::option(  0,  "", "<img>", base::cmd_line::arg<0,1>, "Image to show", true )
	);

	auto errhandler = base::make_guard( [&]() { std::cerr << options << std::endl; } );
	options.parse( argc, argv );
	errhandler.dismiss();

	base::file_system::add( "file", std::make_shared<base::posix_file_system>() );

	app = std::make_shared<gui::application>();
	app->push();

	auto win = app->new_window();
	win->set_title( app->active_platform() );

	auto viewer = std::make_shared<viewer::viewer>();
	win->set_widget( viewer );

	win->show();

	if ( auto &opt = options["<img>"] )
	{
		auto context = win->bind();

		auto v = opt.value();
		media::container c = media::exr_reader( base::uri( "file", "", v ) );
		auto t = std::dynamic_pointer_cast<media::video_track>( c.at( 0 ) );
		auto f = t->at( t->begin() );

		auto &chan = f->at( 0 );

		auto txt = std::make_shared<gl::texture>();
		{
			auto tbind = txt->bind( gl::texture::target::TEXTURE_RECTANGLE );
			tbind.image_2d_rgba( gl::format::RGBA_HALF, chan.width(), chan.height(), gl::image_type::HALF, chan.data() );
		}
		viewer->set_texture_a( txt );
	}

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

