
#include <iostream>
#include <sstream>
#include <random>
#include <functional>

#include <gui/application.h>
#include <gui/cocoa_style.h>
#include <gui/container.h>
#include <gui/layouts.h>
#include <gui/label.h>
#include <gui/button.h>
#include <gui/slider.h>
#include <gui/scroll_area.h>

namespace {

////////////////////////////////////////

std::shared_ptr<gui::widget> build_form( void )
{
	auto container = std::make_shared<gui::container<gui::form_layout>>( direction::DOWN );
	container->set_spacing( 12, 6 );
	container->set_pad( 12.5, 12.5, 12.5, 12.5 );
	container->add( std::make_shared<gui::label>( "Hello World" ), std::make_shared<gui::button>( "Click Me" ) );
	container->add( std::make_shared<gui::label>( "What" ), std::make_shared<gui::slider>() );
	return container;
}

////////////////////////////////////////

std::shared_ptr<gui::widget> build_grid( void )
{
	auto container = std::make_shared<gui::container<gui::grid_layout>>();
	container->set_spacing( 12, 6 );
	container->set_pad( 12.5, 12.5, 12.5, 12.5 );

	std::string name( "A" );
	std::vector<std::shared_ptr<gui::widget>> row;
	for ( size_t y = 0; y < 5; ++y )
	{
		row.clear();
		for ( size_t x = 0; x < 5; ++x )
		{
			row.emplace_back( new gui::label( name, alignment::CENTER ) );
			++name[0];
		}
		container->add_row( row );
	}
	container->set_row_weight( 0, 0.25 );
	container->set_row_weight( 1, 0.5 );
	container->set_row_weight( 2, 1.0 );
	container->set_row_weight( 3, 0.5 );
	container->set_row_weight( 4, 0.25 );
	container->set_column_weight( 0, 0.25 );
	container->set_column_weight( 1, 0.5 );
	container->set_column_weight( 2, 1.0 );
	container->set_column_weight( 3, 0.5 );
	container->set_column_weight( 4, 0.25 );

	return container;
}

////////////////////////////////////////

int safemain( int argc, char **argv )
{
	auto app = std::make_shared<gui::application>();
	app->push();
	app->set_style( std::make_shared<gui::cocoa_style>() );

	auto win = app->new_window();

	std::string test = "form";
	if ( argc > 1 )
		test = argv[1];

	if ( test == "form" )
		win->set_widget( build_form() );
	else if ( test == "grid" )
		win->set_widget( build_grid() );

	win->show();

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

