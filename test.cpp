
#include <iostream>
#include <unistd.h>

#include <core/contract.h>
#include <platform/xcb/system.h>

namespace {

int safemain( int argc, char **argv )
{
	auto sys = std::make_shared<xcb::system>();
	std::cout << sys->name() << " - " << sys->description() << std::endl;

	auto screens = sys->screens();
	for ( auto s: screens )
		std::cout << ' ' << s->bounds().width() << 'x' << s->bounds().height() << std::endl;

	auto win = sys->new_window();

	draw::path round_rect;
	round_rect.rounded_rect( { 10.5, 10.5 }, { 350.5, 100.5 }, 10 );

//	std::shared_ptr<platform::points> ps;
	auto draw_stuff = [&]
	{
		std::cout << "Drawing" << std::endl;
		auto canvas = win->canvas();
		canvas->fill( draw::color( 0, 0, 0 ) );
		canvas->draw_path( round_rect, draw::color( 1, 1, 1 ) );
		canvas->present();
	};

	win->resize( 400, 400 );
	win->set_title( "Hello World" );
	win->show();

	win->when_exposed( draw_stuff );

	auto keypress = [&]( platform::scancode sc )
	{
		std::cout << "Key pressed " << static_cast<uint32_t>(sc) << std::endl;
	};

	auto kbd = sys->get_keyboard();
	kbd->when_pressed( keypress );

	auto dispatcher = sys->get_dispatcher();
	dispatcher->execute();
	return 0;
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

