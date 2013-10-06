
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
#include <gui/tight_constraint.h>
#include <gui/flow_constraint.h>

#include <core/any.h>

int main( int argc, char **argv )
{
	any a = int(1);
	any b = "Hello";

	std::cout << a.as<int>();
	std::cout << ' ' << b.as<char*>() << std::endl;

	allegro::system sys;
	al_set_new_display_flags( ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE );

	allegro::event_queue queue;

	allegro::display win( 640, 480 );
	win.set_title( "Hello World" );
	queue.add_source( win );

	allegro::keyboard keyb;
	queue.add_source( keyb );

	allegro::timer timer( 1.0 );
	queue.add_source( timer );

	allegro::color black( 0, 0, 0 );
	allegro::color white( 255, 255, 255 );

	container c;

	auto left_area = c.new_area();
	auto right_area = c.new_area();
	flow_constraint columns( c.bounds(), direction::LEFT );
	columns.add_area( left_area, 0.5 );
	columns.add_area( right_area, 1.0 );

	tight_constraint left( left_area, orientation::HORIZONTAL );
	tight_constraint right( right_area, orientation::HORIZONTAL );
	std::vector<tight_constraint> rows;
	flow_constraint down( c.bounds(), direction::DOWN );

	auto add = [&]
	{
		auto box = c.new_area();
		auto a = c.new_area();
		auto b = c.new_area();

		rows.emplace_back( box, orientation::VERTICAL );
		rows.back().add_area( a );
		rows.back().add_area( b );

		down.add_area( box );

		a->set_minimum_width( 100 );
		a->set_minimum_height( 24 );
		b->set_minimum_width( 100 );
		b->set_minimum_height( 24 );

		left.add_area( a );
		right.add_area( b );
	};

	for ( size_t i = 0; i < 5; ++i )
		add();
	
	auto recompute_layout = [&] ( double w, double h ) {
		c.bounds()->set_horizontal( 0, w );
		c.bounds()->set_vertical( 0, h );
		for ( auto t: rows )
			t.recompute_minimum();
		left.recompute_minimum();
		right.recompute_minimum();
		down.recompute_minimum();
		columns.recompute_minimum();

		columns.recompute_constraint();
		down.recompute_constraint();
		right.recompute_constraint();
		left.recompute_constraint();
		for ( auto t: rows )
			t.recompute_constraint();
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
				for ( auto a: c )
				{
					std::cout << a->x1() << ',' << a->y1() << " - " << a->x2() << ',' << a->y2() << std::endl;
					t.draw_rect( a->x1(), a->y1(), a->x2(), a->y2(), black, 1.F );
				}
				std::cout << std::endl;
				t.flip();
				break;
			}

			default:
				break;
		}
	} while ( !done );

	return 0;
}
