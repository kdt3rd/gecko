
#include <iostream>

#include <allegro/system.h>
#include <allegro/color.h>
#include <allegro/target.h>
#include <allegro/display.h>
#include <allegro/keyboard.h>
#include <allegro/timer.h>
#include <allegro/event.h>
#include <allegro/event_queue.h>

#include <gui/container.h>
#include <gui/form_layout.h>
#include <gui/box_layout.h>

namespace {
int safemain( int argc, char **argv )
{
	allegro::system sys;
	al_set_new_display_flags( ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE );

	allegro::event_queue queue;

	allegro::display win( 640, 480 );
	win.set_title( "Hello World" );
	queue.add_source( win );

	allegro::keyboard keyb;
	queue.add_source( keyb );

	allegro::timer timer( 0.1 );
	queue.add_source( timer );

	allegro::color black( 0, 0, 0 );
	allegro::color white( 255, 255, 255 );

	std::shared_ptr<container> c = std::make_shared<container>();

	box_layout layout( c, direction::DOWN );

	auto add = [&]
	{
		auto a = layout.new_area();

		a->set_minimum_width( 100 );
		a->set_minimum_height( 24 );
	};

	for ( size_t i = 0; i < 5; ++i )
		add();
	
	auto recompute_layout = [&] ( double w, double h ) {
		c->bounds()->set_horizontal( 0, w );
		c->bounds()->set_vertical( 0, h );

		layout.recompute_minimum();
		layout.recompute_layout();
	};

	recompute_layout( 640, 480 );

	timer.start();
	bool done = false;
	do
	{
		allegro::event ev;
		queue.wait( ev );
		switch ( ev.type() )
		{
			case allegro::EVENT_KEY_DOWN:
			case allegro::EVENT_DISPLAY_CLOSE:
				done = true;
				break;

			case allegro::EVENT_DISPLAY_RESIZE:
				recompute_layout( ev.width(), ev.height() );
				ev.acknowledge_resize();
				break;

			case allegro::EVENT_TIMER:
			{
				allegro::target t( win );
				t.clear( white );
				for ( auto a: *c )
					t.draw_rect( a->x1() + 0.5, a->y1() + 0.5, a->x2() - 0.5, a->y2() - 0.5, black, 1.F );
				t.flip();
				break;
			}

			default:
				break;
		}
	} while ( !done );

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

