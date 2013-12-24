
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
#include <gui/color.h>
#include <gui/scroll_area.h>
#include <gui/tree_node.h>

namespace {

////////////////////////////////////////

std::shared_ptr<gui::widget> build_form( direction dir )
{
	auto container = std::make_shared<gui::form>( dir );
	container->set_spacing( 12, 6 );
	container->set_pad( 12.5, 12.5, 12.5, 12.5 );
	container->add( std::make_shared<gui::label>( "Hello World" ), std::make_shared<gui::button>( "Click Me" ) );
	container->add( std::make_shared<gui::label>( "What" ), std::make_shared<gui::slider>() );
	return container;
}

////////////////////////////////////////

std::shared_ptr<gui::widget> build_grid( direction dir )
{
	auto container = std::make_shared<gui::grid>( dir );
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

std::shared_ptr<gui::widget> build_box( direction dir )
{
	auto container = std::make_shared<gui::simple_container>( dir );
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

std::shared_ptr<gui::widget> build_tree( direction dir )
{
	auto container = std::make_shared<gui::tree_node>( 24.0, dir );
	container->set_root( std::make_shared<gui::label>( "Root" ) );
	container->set_spacing( 12, 6 );
	container->set_pad( 12.5, 12.5, 12.5, 12.5 );

	std::string name( "A" );
	for ( size_t y = 0; y < 3; ++y )
	{
		auto node = std::make_shared<gui::tree_node>( 24.0, dir );
		if( y == 1 )
		{
			auto button = std::make_shared<gui::button>( "+" );
			button->when_activated += [=]( void )
			{
				node->set_collapsed( !node->collapsed() );
			};
			node->set_root( button );
		}
		else
			node->set_root( std::make_shared<gui::label>( name ) );
		std::string sub( "1" );
		for ( size_t x = 0; x < 3; ++x )
		{
			node->add( std::make_shared<gui::label>( sub ) );
			node->set_spacing( 12, 6 );
			++sub[0];
		}
		container->add( node );
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

	std::string dirname = "box";
	if ( argc > 2 )
		dirname = argv[2];
	direction dir = direction::DOWN;
	if ( dirname == "down" )
		dir = direction::DOWN;
	else if ( dirname == "up" )
		dir = direction::UP;
	else if ( dirname == "left" )
		dir = direction::LEFT;
	else if ( dirname == "right" )
		dir = direction::RIGHT;

	if ( test == "form" )
		win->set_widget( build_form( dir ) );
	else if ( test == "grid" )
		win->set_widget( build_grid( dir ) );
	else if ( test == "box" )
		win->set_widget( build_box( dir ) );
	else if ( test == "tree" )
		win->set_widget( build_tree( dir ) );

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

