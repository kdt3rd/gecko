
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

	auto draw_stuff = [&]
	{
		auto painter = win->paint();
		/*
		std::vector<point> ls;
		painter->set_color( color( 1.0, 1.0, 1.0 ) );
		for ( size_t i = 0; i < 5; ++i )
			ls.emplace_back( 10 + i * 5, 10 + i * 5 ); //, 5, 5 );
		painter->draw_points( ls.data(), ls.size() );
		*/
		painter->present();
	};
	win->when_exposed( draw_stuff );

	win->resize( 320, 240 );
	win->set_title( "Hello World" );
	win->show();

	draw_stuff();

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

