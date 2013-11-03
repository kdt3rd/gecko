
#include <sstream>
#include <random>
#include <functional>

#include <layout/form_layout.h>
#include <layout/box_layout.h>
#include <layout/grid_layout.h>
#include <layout/tree_layout.h>
#include <gui/application.h>
#include <gui/cocoa_style.h>

namespace {

////////////////////////////////////////

void build_form_layout( const std::shared_ptr<gui::window> &win )
{
	gui::builder builder( win );
	auto layout = builder.new_layout<layout::form_layout>( layout::direction::RIGHT );
	layout->set_pad( 12.0, 12.0, 12.0, 12.0 );
	layout->set_spacing( 12.0, 12.0 );

	auto row = layout->new_row();
	builder.make_label( row.first, "Hello World" );
	builder.make_button( row.second, "Press Me" );

	row = layout->new_row();
	builder.make_label( row.first, "Goodbye World" );
	builder.make_button( row.second, "Me Too" );
}

void build_box_layout( const std::shared_ptr<gui::window> &win )
{
	gui::builder builder( win );
	auto layout = builder.new_layout<layout::box_layout>( layout::direction::DOWN );
	layout->set_pad( 12.0, 12.0, 12.0, 12.0 );
	layout->set_spacing( 12.0, 12.0 );

	builder.make_label( layout->new_area(), "Hello World" );
	builder.make_button( layout->new_area(), "Press Me" );

	builder.make_label( layout->new_area(), "Goodbye World" );
	builder.make_button( layout->new_area(), "Me Too" );
}

void build_grid_layout( const std::shared_ptr<gui::window> &win )
{
	gui::builder builder( win );
	auto layout = builder.new_layout<layout::grid_layout>();
	layout->set_pad( 12.0, 12.0, 12.0, 12.0 );
	layout->set_spacing( 12.0, 12.0 );

	for ( size_t i = 0; i < 5; ++i )
		layout->new_column( 1.0 );

	int count = 0;
	for ( size_t i = 0; i < 5; ++i )
	{
		auto cols = layout->new_row();
		for ( auto a: cols )
		{
			std::stringstream tmp;
			tmp << ++count;
			builder.make_label( a, tmp.str() );
		}
	}
}

void build_tree_layout( const std::shared_ptr<gui::window> &win )
{
	gui::builder builder( win );
	auto layout = builder.new_layout<layout::tree_layout>( 24.0 );
	layout->set_pad( 12.0, 12.0, 12.0, 12.0 );
	layout->set_spacing( 12.0 );

	std::random_device rd;
	std::default_random_engine rand( rd() );
	std::uniform_int_distribution<int> uniform_dist( 1, 6 );

	std::function<void(std::shared_ptr<layout::tree_layout>,int)> make_tree =
		[&]( std::shared_ptr<layout::tree_layout> l, int level )
		{
			int count = uniform_dist( rand );
			for ( int i = 0; i < count; ++i )
			{
				std::stringstream tmp;
				tmp << char( 'A' + i );
				builder.make_label( l->new_area( 0.0 ), tmp.str() );
				if ( level < 2 )
					make_tree( l->new_branch( 0.0 ), level + 1 );
			}
		};

	make_tree( layout, 0 );
}

////////////////////////////////////////

int safemain( int argc, char **argv )
{
	auto app = std::make_shared<gui::application>();
	app->push();
	app->set_style( std::make_shared<gui::cocoa_style>() );

	auto win1 = app->new_window();
	build_form_layout( win1 );
	win1->show();

	auto win2 = app->new_window();
	build_box_layout( win2 );
	win2->show();

	auto win3 = app->new_window();
	build_grid_layout( win3 );
	win3->show();

	auto win4 = app->new_window();
	build_tree_layout( win4 );
	win4->show();

	int code = app->run();
	app->pop();
	return code;
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

