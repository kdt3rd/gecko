
#include <sstream>

#include <layout/form_layout.h>
#include <layout/box_layout.h>
#include <layout/grid_layout.h>
#include <gui/application.h>
#include <gui/cocoa_style.h>

namespace {

////////////////////////////////////////

void build_form_window( const std::shared_ptr<gui::window> &win )
{
	using layout::form_layout;

	gui::builder builder( win );
	auto layout = builder.new_layout<form_layout>( layout::direction::RIGHT );
	layout->set_pad( 12.0, 12.0, 12.0, 12.0 );
	layout->set_spacing( 12.0, 12.0 );

	auto row = layout->new_row();
	builder.make_label( row.first, "Hello World" );
	builder.make_button( row.second, "Press Me" );

	row = layout->new_row();
	builder.make_label( row.first, "Goodbye World" );
	builder.make_button( row.second, "Me Too" );
}

void build_box_window( const std::shared_ptr<gui::window> &win )
{
	using layout::box_layout;

	gui::builder builder( win );
	auto layout = builder.new_layout<box_layout>( layout::direction::DOWN );
	layout->set_pad( 12.0, 12.0, 12.0, 12.0 );
	layout->set_spacing( 12.0, 12.0 );

	builder.make_label( layout->new_area(), "Hello World" );
	builder.make_button( layout->new_area(), "Press Me" );

	builder.make_label( layout->new_area(), "Goodbye World" );
	builder.make_button( layout->new_area(), "Me Too" );
}

void build_grid_window( const std::shared_ptr<gui::window> &win )
{
	using layout::grid_layout;

	gui::builder builder( win );
	auto layout = builder.new_layout<grid_layout>();
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

////////////////////////////////////////

int safemain( int argc, char **argv )
{
	auto app = std::make_shared<gui::application>();
	app->push();
	app->set_style( std::make_shared<gui::cocoa_style>() );

	auto win1 = app->new_window();
	build_form_window( win1 );
	win1->show();

	auto win2 = app->new_window();
	build_box_window( win2 );
	win2->show();

	auto win3 = app->new_window();
	build_grid_window( win3 );
	win3->show();

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

