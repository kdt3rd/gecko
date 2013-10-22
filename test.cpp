
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

	std::shared_ptr<platform::points> ps;
	auto draw_stuff = [&]
	{
		auto painter = win->paint();
		if ( !ps )
			ps = painter->make_star( 200.0, 200.0, 150.0, 9, 4 );

		painter->set_color( draw::color( 0.0, 0.0, 0.0 ) );
		painter->clear();

		painter->set_color( draw::color( 1.0, 0.0, 1.0 ) );
		painter->fill_polygon( ps );
		painter->present();
	};
	win->when_exposed( draw_stuff );

	win->resize( 400, 400 );
	win->set_title( "Hello World" );
	win->show();

	draw_stuff();

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

