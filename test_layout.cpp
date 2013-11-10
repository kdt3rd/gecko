
#include <iostream>
#include <map>
#include <functional>

#define PLATFORM_H <platform/PLATFORM/system.h>
#include PLATFORM_H

#include <layout/form_layout.h>
#include <layout/box_layout.h>
#include <layout/grid_layout.h>
#include <layout/tree_layout.h>

namespace {

////////////////////////////////////////

std::shared_ptr<layout::layout> test_box( const std::shared_ptr<draw::area> &c )
{
	auto l =  std::make_shared<layout::box_layout>( c, direction::DOWN );
	for ( size_t i = 0; i < 5; ++i )
	{
		auto a = l->new_area( i == 2 ? 1.0 : 0.0 );
		a->set_minimum( 100, 24 );
	}
	return l;
}

////////////////////////////////////////

std::shared_ptr<layout::layout> test_tree( const std::shared_ptr<draw::area> &c )
{
	auto l = std::make_shared<layout::tree_layout>( c, 24.0 );
	for ( size_t i = 0; i < 2; ++i )
	{
		auto a = l->new_area( 0.0 );
		a->set_minimum( 100, 24 );
	}
	auto b1 = l->new_branch();
	for ( size_t i = 0; i < 2; ++i )
	{
		auto a = b1->new_area();
		a->set_minimum( 100, 24 );
	}
	for ( size_t i = 0; i < 2; ++i )
	{
		auto a = l->new_area( 0.0 );
		a->set_minimum( 100, 24 );
	}
	l->new_area( 1.0 );

	return l;
}

////////////////////////////////////////

std::shared_ptr<layout::layout> test_form( const std::shared_ptr<draw::area> &c )
{
	std::shared_ptr<draw::area> a, b;

	auto l = std::make_shared<layout::form_layout>( c );

	for ( size_t i = 0; i < 5; ++i )
	{
		std::tie( a, b ) = l->new_row();
		a->set_minimum( 100, 24 );
		b->set_minimum( 100, 24 );
	}

	return l;
}

////////////////////////////////////////

std::shared_ptr<layout::layout> test_grid( std::shared_ptr<draw::area> c )
{
	auto l = std::make_shared<layout::grid_layout>( c );

	for ( size_t i = 0; i < 5; ++i )
		auto tmp = l->new_column( i == 2 ? 1.0 : 0.0 );

	for ( size_t i = 0; i < 5; ++i )
	{
		auto tmp = l->new_row( i == 2 ? 1.0 : 0.0 );
		for ( auto a: tmp )
			a->set_minimum( 50, 24 );
	}

	return l;
}

////////////////////////////////////////

std::map<std::string,std::function<std::shared_ptr<layout::layout>(std::shared_ptr<draw::area>)>> tests =
{
	{ "grid", test_grid },
	{ "box", test_box },
	{ "form", test_form },
	{ "tree", test_tree },
};

////////////////////////////////////////

int safemain( int argc, char **argv )
{
	precondition( argc > 1, "expected argument" );

	auto sys = std::make_shared<platform::native_system>();
	auto win = sys->new_window();

	auto c = std::make_shared<draw::area>();

	auto layout = tests[argv[1]]( c );

//	auto fontmgr = sys->get_font_manager();
//	auto font = fontmgr->get_font( "ubuntu", "bold", 48.0 );
//	if ( !font )
//		throw std::runtime_error( "font not found" );

	draw::gradient grad;
	grad.add_stop( 0.0, draw::color( 0.6, 0.6, 0.6 ) );
	grad.add_stop( 1.0, draw::color( 0.3, 0.3, 0.3 ) );

	auto redraw_window = [&] ( void )
	{
		auto canvas = win->canvas();
		canvas->fill( draw::color( 0.5, 0.5, 0.5 ) );

		/*
		bool skip = true;
		for ( auto a: *c )
		{
			if ( ( a->width() * a->height() > 0 ) && !skip )
			{
				draw::path p;
				p.rounded_rect( { a->x1() + 0.5, a->y1() + 0.5 }, a->width(), a->height(), 5 );
				draw::paint paint( draw::color( 1, 1, 1 ) );
				paint.set_fill_linear( { a->x1(), a->y1() }, 0, a->height(), grad );
				canvas->draw_path( p, paint );
			}
			else
				skip = false;
		}
		*/
//		draw::paint paint2( { 1, 1, 1, 0 } );
//		paint2.set_fill_color( { 1, 1, 1 } );
//		canvas->draw_text( font, { 50, 150 }, "Hell0 World!", paint2 );
		canvas->present();
	};

	auto recompute_layout = [&] ( double w, double h )
	{
		layout->recompute_minimum();

		if ( w < c->minimum_width() )
			w = c->minimum_width();
		if ( h < c->minimum_height() )
			h = c->minimum_height();
		c->set_horizontal( 0, w );
		c->set_vertical( 0, h );

		layout->recompute_layout();
	};

	recompute_layout( 640, 480 );

	win->resized.callback( recompute_layout );
	win->exposed.callback( redraw_window );

	win->set_title( "Hello World" );
	win->resize( 640, 480 );
	win->show();

	std::shared_ptr<platform::timer> t = sys->new_timer();
	t->elapsed.callback( [&]
	{
		t->schedule( 10.0 );
	} );

//	win->when_entered( [&] { std::cout << "Entered!" << std::endl; } );
//	win->when_exited( [&] { std::cout << "Exited!" << std::endl; } );

	t->schedule( 1.0 );

	return sys->get_dispatcher()->execute();
}

////////////////////////////////////////

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

