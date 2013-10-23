
#include <iostream>
#include <map>
#include <functional>

#include <platform/xcb/system.h>

#include <layout/form_layout.h>
#include <layout/box_layout.h>
#include <layout/grid_layout.h>
#include <layout/tree_layout.h>

namespace {

////////////////////////////////////////

std::shared_ptr<layout> test_box( std::shared_ptr<container> c )
{
	auto l =  std::make_shared<box_layout>( c, direction::DOWN );
	for ( size_t i = 0; i < 5; ++i )
	{
		auto a = l->new_area( i == 2 ? 1.0 : 0.0 );
		a->set_minimum( 100, 24 );
	}
	return l;
}

////////////////////////////////////////

std::shared_ptr<layout> test_tree( std::shared_ptr<container> c )
{
	auto l = std::make_shared<tree_layout>( c, 24.0 );
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

std::shared_ptr<layout> test_form( std::shared_ptr<container> c )
{
	std::shared_ptr<area> a, b;

	auto l = std::make_shared<form_layout>( c );

	for ( size_t i = 0; i < 5; ++i )
	{
		std::tie( a, b ) = l->new_row();
		a->set_minimum( 100, 24 );
		b->set_minimum( 100, 24 );
	}

	return l;
}

////////////////////////////////////////

std::shared_ptr<layout> test_grid( std::shared_ptr<container> c )
{
	auto l = std::make_shared<grid_layout>( c );

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

std::map<std::string,std::function<std::shared_ptr<layout>(std::shared_ptr<container>)>> tests =
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

	auto sys = std::make_shared<xcb::system>();
	auto win = sys->new_window();

	std::shared_ptr<container> c = std::make_shared<container>();

	auto layout = tests[argv[1]]( c );

	auto redraw_window = [&] ( void )
	{
		std::cout << "redraw" << std::endl;
		/*
		auto painter = win->paint();

		painter->set_color( draw::color( 0.5, 0.5, 0.5 )  );
		painter->clear();

		auto rs = painter->new_rectangles();
		for ( auto a: *c )
			rs->add_rectangle( a->x1(), a->y1(), a->width()-1, a->height()-1 );
		painter->set_color( draw::color( 1, 0, 1 ) );
		painter->draw_rects( rs );
		painter->present();
		*/
	};

	auto recompute_layout = [&] ( double w, double h )
	{
		std::cout << "resize " << w << ' ' <<  h << std::endl;
		layout->recompute_minimum();
		std::shared_ptr<area> b = c->bounds();

		if ( w < b->minimum_width() )
			w = b->minimum_width();
		if ( h < b->minimum_height() )
			h = b->minimum_height();
		b->set_horizontal( 0, w );
		b->set_vertical( 0, h );

		layout->recompute_layout();
		redraw_window();
	};

	recompute_layout( 640, 480 );

	win->when_resized( recompute_layout );
	win->when_exposed( redraw_window );

	win->set_title( "Hello World" );
	win->resize( 640, 480 );
	win->show();

	std::shared_ptr<platform::timer> t = sys->new_timer();
	t->when_elapsed( [&]
	{
		std::cout << "Timeout!" << std::endl;
		t->schedule( 10.0 );
	} );

	win->when_entered( [&] { std::cout << "Entered!" << std::endl; } );
	win->when_exited( [&] { std::cout << "Exited!" << std::endl; } );

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

