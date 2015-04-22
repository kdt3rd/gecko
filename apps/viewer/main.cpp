
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
		base::cmd_line::option(  0,  "", "<img>", base::cmd_line::arg<0,1>, "Image to show", false )
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

		if ( f->has_channels( "R", "G", "B" ) )
		{
			std::vector<float> img( f->width() * f->height() * 3 );
			float *line = img.data();
			media::image_buffer chans[3];
			chans[0] = f->at( "R" );
			chans[1] = f->at( "G" );
			chans[2] = f->at( "B" );

			for ( size_t y = 0; y < f->height(); ++y )
			{
				chans[0].get_scanline( y, line+0, 3 );
				chans[1].get_scanline( y, line+1, 3 );
				chans[2].get_scanline( y, line+2, 3 );
				line += f->width() * 3;
			}

			auto txt = std::make_shared<gl::texture>();
			{
				auto tbind = txt->bind( gl::texture::target::TEXTURE_RECTANGLE );
				tbind.image_2d_rgb( gl::format::RGBA_FLOAT, f->width(), f->height(), gl::image_type::FLOAT, img.data() );
			}
			viewer->set_texture_a( txt );
		}
		else if ( f->has_channels( "Y" ) )
		{
			std::vector<float> img( f->width() * f->height() );
			float *line = img.data();
			media::image_buffer chan = f->at( "Y" );

			for ( size_t y = 0; y < f->height(); ++y )
			{
				chan.get_scanline( y, line );
				line += f->width();
			}

			auto txt = std::make_shared<gl::texture>();
			{
				auto tbind = txt->bind( gl::texture::target::TEXTURE_RECTANGLE );
				tbind.image_2d_red( gl::format::RGBA_FLOAT, f->width(), f->height(), gl::image_type::FLOAT, img.data() );
				tbind.set_swizzle( gl::swizzle::RED, gl::swizzle::RED, gl::swizzle::RED );
			}
			viewer->set_texture_a( txt );
		}
		else
			throw_logic( "can only view RGB images" );
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

