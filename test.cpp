
#include <iostream>
#include <unistd.h>

#include <core/contract.h>
#include <view/cocoa/button.h>

#define PLATFORM_H <platform/PLATFORM/system.h>
#include PLATFORM_H

namespace {

int safemain( int argc, char **argv )
{
	auto sys = std::make_shared<platform::native_system>();
	std::cout << sys->name() << " - " << sys->description() << std::endl;

	auto fontmgr = sys->get_font_manager();
	std::cout << fontmgr->name() << " - " << fontmgr->version() << std::endl;

	std::cout << "Font families:\n";
	for ( auto &f: fontmgr->get_families() )
		std::cout << "  " << f << '\n';

	auto font = fontmgr->get_font( "Lucida Grande", "bold", 14.0 );
	if ( !font )
		throw std::runtime_error( "font not found" );

	auto screens = sys->screens();
	for ( auto s: screens )
		std::cout << ' ' << s->bounds().width() << 'x' << s->bounds().height() << std::endl;

	auto win = sys->new_window();

	draw::color bg( 0.9294, 0.9294, 0.9294 );
	draw::color fg( 0, 0, 0 );
//	draw::color textbg = bg.change( 0.30 );
//	draw::color textfg = bg.change( -0.64 ).desaturate( -0.10 );

	draw::path round_rect;
	round_rect.rounded_rect( { 10, 10 }, 150, 24, 6 );

	draw::gradient grad;
	{
		draw::color c = bg.change( 0.10 );
		grad.add_stop( 0.0, c );
		grad.add_stop( 0.7, c.change( -0.13 ) );
		grad.add_stop( 1.0, c.change( -0.13 ) );
	}

//	std::shared_ptr<platform::points> ps;
	auto area = std::make_shared<draw::area>( draw::point( 10.5, 10.5 ), 150, 21 );

	cocoa::button b( area );

	auto draw_stuff = [&]
	{
		std::cout << "Drawing" << std::endl;
		auto canvas = win->canvas();
		canvas->fill( bg );

		b.paint( canvas );

		draw::paint paint2( { 0.1961, 0.1961, 0.1961, 0 } );
		paint2.set_fill_color( { 0.1961, 0.1961, 0.1961 } );
		canvas->draw_text( font, { 18, 10+16 }, "Hello", paint2 );

		canvas->present();

	};

	win->when_exposed( draw_stuff );
	win->resize( 400, 400 );
	win->set_title( "Hello World" );
	win->show();


	auto dispatcher = sys->get_dispatcher();

	auto keypress = [&]( const std::shared_ptr<platform::keyboard> &k, platform::scancode sc )
	{
		auto canvas = win->canvas();
		canvas->screenshot_png( "test.png" );
		std::cout << "Screenshot " << static_cast<uint32_t>(sc) << std::endl;
		dispatcher->exit( 0 );
	};

	auto mousepress = [&]( const std::shared_ptr<platform::mouse> &m, const draw::point &p, int b )
	{
		std::cout << "Press: " << b << std::endl;
	};

	auto mousemove = [&]( const std::shared_ptr<platform::mouse> &m, const draw::point &p )
	{
		std::cout << "Moved: " << p.x() << ',' << p.y() << std::endl;
	};

	win->when_key_pressed( keypress );

	win->when_mouse_pressed( mousepress );
	win->when_mouse_moved( mousemove );

	return dispatcher->execute();
}

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
		print_exception( std::cerr, e );
	}
	return ret;
}

////////////////////////////////////////

