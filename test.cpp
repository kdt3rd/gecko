
#include <iostream>
#include <unistd.h>

#include <core/contract.h>
#include <platform/xcb/system.h>

namespace {

int safemain( int argc, char **argv )
{
	auto sys = std::make_shared<xcb::system>();
	std::cout << sys->name() << " - " << sys->description() << std::endl;

	auto fontmgr = sys->get_font_manager();
	std::cout << fontmgr->name() << " - " << fontmgr->version() << std::endl;

	std::cout << "Font families:\n";
	for ( auto &f: fontmgr->get_families() )
		std::cout << "  " << f << '\n';

	std::cout << "Font styles:\n";
	for ( auto &f: fontmgr->get_styles() )
		std::cout << "  " << f << '\n';

	auto screens = sys->screens();
	for ( auto s: screens )
		std::cout << ' ' << s->bounds().width() << 'x' << s->bounds().height() << std::endl;

	auto win = sys->new_window();

	draw::color bg( 0.188, 0.188, 0.188 );
	draw::color fg = bg.change( 0.64 ).desaturate( -0.10 );
	draw::color textbg = bg.change( 0.30 );
	draw::color textfg = bg.change( -0.64 ).desaturate( -0.10 );

	draw::path round_rect;
	round_rect.rounded_rect( { 10.5, 10.5 }, 150, 24, 6 );

	draw::gradient grad;
	{
		draw::color c = bg.change( 0.10 );
		grad.add_stop( 0.0, c );
		grad.add_stop( 0.7, c.change( -0.13 ) );
		grad.add_stop( 1.0, c.change( -0.13 ) );
	}

//	std::shared_ptr<platform::points> ps;
	auto draw_stuff = [&]
	{
		std::cout << "Drawing" << std::endl;
		auto canvas = win->canvas();
		canvas->fill( bg );

		draw::paint paint( draw::color( 1, 1, 1 ) );
		paint.set_fill_linear( { 10.5, 10.5 }, 0, 24, grad );
		canvas->draw_path( round_rect, paint );
		canvas->present();
	};

	win->resize( 400, 400 );
	win->set_title( "Hello World" );
	win->show();

	win->when_exposed( draw_stuff );

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

