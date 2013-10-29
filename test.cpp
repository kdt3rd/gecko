
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

	std::cout << "Font foundries:\n";
	for ( auto &f: fontmgr->get_foundries() )
		std::cout << "  " << f << '\n';

	std::cout << "Font families:\n";
	for ( auto &f: fontmgr->get_families() )
		std::cout << "  " << f << '\n';

	std::cout << "Font styles:\n";
	for ( auto &f: fontmgr->get_styles() )
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
	cocoa::button b;
	auto draw_stuff = [&]
	{
		std::cout << "Drawing" << std::endl;
		auto canvas = win->canvas();
		canvas->fill( bg );

		b.paint( canvas, draw::rect( { 10.5, 10.5 }, 150, 21 ) );

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

	auto keypress = [&]( platform::scancode sc )
	{
		std::cout << "Key pressed " << static_cast<uint32_t>(sc) << std::endl;
		dispatcher->exit( 0 );
	};

	auto kbd = sys->get_keyboard();
	kbd->when_pressed( keypress );

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

