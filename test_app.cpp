
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
	auto container = std::make_shared<gui::form>( direction::DOWN );
	container->set_spacing( 12, 6 );
	container->set_pad( 12.5, 12.5, 12.5, 12.5 );
	container->add( std::make_shared<gui::label>( "Hello World" ), std::make_shared<gui::button>( "Click Me" ) );
	container->add( std::make_shared<gui::label>( "What" ), std::make_shared<gui::slider>() );
	return container;
}

////////////////////////////////////////

std::shared_ptr<gui::widget> build_grid( void )
{
	auto container = std::make_shared<gui::grid>();
	container->set_spacing( 12, 6 );
	container->set_pad( 12.5, 12.5, 12.5, 12.5 );

	std::string name( "A" );
	std::vector<std::shared_ptr<gui::widget>> row;
	const double weights[5] = { 0.25, 0.5, 1.0, 0.5, 0.25 };
	for ( size_t y = 0; y < 5; ++y )
	{
		row.clear();
		for ( size_t x = 0; x < 5; ++x )
		{
			row.push_back( std::make_shared<gui::label>( name, alignment::CENTER ) );
			++name[0];
		}
		container->add_row( row, weights[y] );
	}
	for ( size_t y = 0; y < 5; ++y )
		container->set_column_weight( y, weights[y] );

	return container;
}

////////////////////////////////////////

std::shared_ptr<gui::widget> build_box( void )
{
	auto container = std::make_shared<gui::simple_container>( direction::DOWN );
	container->set_spacing( 12, 6 );
	container->set_pad( 12.5, 12.5, 12.5, 12.5 );

	std::string name( "A" );
	std::vector<std::shared_ptr<gui::widget>> row;
	for ( size_t y = 0; y < 5; ++y )
	{
		container->add( std::make_shared<gui::label>( name, alignment::CENTER ) );
		++name[0];
	}

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
	else if ( test == "box" )
		win->set_widget( build_box() );

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

