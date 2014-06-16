
#include <iostream>
#include <unistd.h>

#include <core/contract.h>
#include <platform/platform.h>
#include <platform/system.h>
#include <draw/object.h>
#include <draw/polylines.h>

namespace {

int safemain( int argc, char **argv )
{
	auto const &platforms = platform::platform::list();
	if ( platforms.empty() )
		throw std::runtime_error( "no platforms available" );

	const platform::platform &p = platforms.front();
	std::cout << "Using platform: " << p.name() << ' ' << p.render() << std::endl;
	auto sys = p.create();
	auto dispatcher = sys->get_dispatcher();

	auto win = sys->new_window();

	core::color bg( 0.9294, 0.9294, 0.9294 );
	core::color fg( 0, 0, 0 );

	core::path path;
	path.rectangle( { 10, 10 }, 150, 24 );

	core::path path2;
	path2.circle( { 180, 22 }, 12 );

	core::paint paint( { 1, 0, 0 }, 3.0 );
	paint.set_fill_color( { 0, 0, 1 } );

	std::shared_ptr<draw::object> obj;
	std::shared_ptr<draw::object> obj2;

	auto draw_stuff = [&]
	{
		auto canvas = win->canvas();
		glViewport( 0, 0, win->width(), win->height() );
		canvas->clear_color( bg );
		canvas->clear();
		canvas->ortho( 0, win->width(), 0, win->height() );

		if ( !obj )
		{
			obj = std::make_shared<draw::object>();
			obj->create( canvas, path, paint );
		}

		if ( !obj2 )
		{
			obj2 = std::make_shared<draw::object>();
			obj2->create( canvas, path2, paint );
		}

		obj->draw( *canvas );
		obj2->draw( *canvas );
	};

	win->exposed.callback( draw_stuff );
	win->resize( 400, 400 );
	win->set_title( "Hello World" );
	win->show();

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

