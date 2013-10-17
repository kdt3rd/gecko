
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
	win->resize( 320, 240 );
	win->set_title( "Hello World" );
	win->show();

	auto painter = win->paint();
	std::vector<line> ls;
	for ( size_t i = 0; i < 5; ++i )
		ls.emplace_back( 10 + i * 5, 10 + i * 5, 5, 5 );
	painter->draw_lines( ls.data(), ls.size() );
	painter->present();

	sleep( 5 );

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

