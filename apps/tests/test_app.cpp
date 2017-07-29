//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <iostream>
#include <sstream>
#include <random>
#include <functional>
#include <memory>

#include <gui/application.h>
#include <gui/window.h>
#include <gui/container.h>
#include <gui/label.h>
#include <gui/button.h>
#include <base/contract.h>
//#include <gui/dark_style.h>
//#include <gui/container.h>
//#include <gui/layouts.h>
//#include <gui/label.h>
//#include <gui/button.h>
//#include <gui/slider.h>
//#include <gui/background_color.h>
//#include <gui/scroll_area.h>
//#include <gui/tree_node.h>
//#include <gui/line_edit.h>
//#include <gui/color_picker.h>

namespace {

static constexpr double padding = 12;
static std::shared_ptr<gui::application> app;
//static std::shared_ptr<gui::menu> extra;

////////////////////////////////////////

/*
std::shared_ptr<gui::form> build_form( direction dir )
{
	auto container = std::make_shared<gui::form>( dir );
	container->set_spacing( 12, 6 );
	container->set_pad( padding, padding, padding, padding );

	auto label = std::make_shared<gui::label>( "Hello World" );
	auto button = std::make_shared<gui::button>( "Click Me" );
	button->when_activated.connect( [=]()
	{
		label->set_text( "Goodbye World" );
	} );

	auto popup = std::make_shared<gui::button>( "Pop up" );
	popup->when_activated.connect( []()
	{
		if ( !extra )
		{
			extra = app->new_menu();
			extra->move( 150, 150 );
			extra->add_entry( "Menu 1" );
			extra->add_entry( "Menu 2" );
			extra->add_entry( "Menu 3" );
		}
		extra->show();
	} );

	auto slider1 = std::make_shared<gui::slider>( 0.5 );
	auto slider2 = std::make_shared<gui::slider>( 0.5 );

	slider1->when_changed.connect( [=]( double x )
	{
		slider2->set_value( x );
	} );

	slider2->when_changing.connect( [=]( double x )
	{
		slider1->set_value( x );
	} );


	container->add( label, button );
	container->add( std::make_shared<gui::label>( "Button" ), popup );
	container->add( std::make_shared<gui::label>( "What" ), slider1 );
	container->add( std::make_shared<gui::label>( "Who" ), slider2 );
	return container;
}
*/

////////////////////////////////////////

/*
std::shared_ptr<gui::background> build_grid( direction dir )
{
	auto container = std::make_shared<gui::grid>( dir );
	container->set_spacing( 12, 6 );
	container->set_pad( padding, padding, padding, padding );

	std::string name( "A" );
	std::vector<std::shared_ptr<gui::widget>> row;
	const double weights[5] = { 0.25, 0.5, 1.0, 0.5, 0.25 };
	for ( size_t y = 0; y < 5; ++y )
	{
		row.clear();
		for ( size_t x = 0; x < 5; ++x )
		{
			row.push_back( std::make_shared<gui::label>( name, base::alignment::CENTER ) );
			++name[0];
		}
		container->add_row( row, weights[y] );
	}
	for ( size_t y = 0; y < 5; ++y )
		container->set_column_weight( y, weights[y] );

	auto bg = std::make_shared<gui::background_color>();// { 1, 0, 1, 1 }, container );
	bg->set_color( { 1, 0, 1, 1 } );
	bg->set_widget( container );

	return bg;
}
*/

////////////////////////////////////////

/*
std::shared_ptr<gui::simple_container> build_box( direction dir )
{
	auto container = std::make_shared<gui::simple_container>( dir );
	container->set_spacing( 12, 6 );
	container->set_pad( padding, padding, padding, padding );

	std::string name( "A" );
	std::vector<std::shared_ptr<gui::widget>> row;
	for ( size_t y = 0; y < 5; ++y )
	{
		container->add( std::make_shared<gui::label>( name, base::alignment::CENTER ) );
		++name[0];
	}

	return container;
}
*/

////////////////////////////////////////

/*
std::shared_ptr<gui::tree_node> build_tree( direction dir )
{
	auto container = std::make_shared<gui::tree_node>( 24.0, dir );
	container->set_root( std::make_shared<gui::label>( "Root" ) );
	container->set_spacing( 12, 6 );
	container->set_pad( padding, padding, padding, padding );

	std::string name( "A" );
	for ( size_t y = 0; y < 3; ++y )
	{
		auto node = std::make_shared<gui::tree_node>( 24.0, dir );
		if( y == 1 )
		{
			auto button = std::make_shared<gui::button>( "+" );
			button->when_activated.connect( [=]( void )
			{
				node->set_collapsed( !node->collapsed() );
			} );
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
	*/


////////////////////////////////////////

/*
std::shared_ptr<gui::simple_container> build_edit( direction dir )
{
	auto container = std::make_shared<gui::simple_container>( dir );
	container->set_spacing( 12, 6 );
	container->set_pad( padding, padding, padding, padding );

	container->add( std::make_shared<gui::label>( "Edit" ) );
	container->add( std::make_shared<gui::line_edit>( "Some text" ) );
	return container;
}
*/

////////////////////////////////////////

/*
std::shared_ptr<gui::widget> build_all( direction dir )
{
	auto container = std::make_shared<gui::simple_container>( dir );
	container->set_spacing( 12, 6 );

	std::vector<std::shared_ptr<gui::widget>> list = { build_form( dir ), build_grid( dir ), build_box( dir ), build_tree( dir ) };

	for ( auto c: list )
		container->add( c );
	container->set_weight( 1, 1.0 );

	auto scroll = std::make_shared<gui::scroll_area>( gui::scroll_behavior::NONE, gui::scroll_behavior::BOUND );
	scroll->set_widget( container );

	return scroll;
}
*/

////////////////////////////////////////

/*
std::shared_ptr<gui::simple_container> build_color( direction dir )
{
	auto container = std::make_shared<gui::simple_container>( dir );
	container->set_spacing( 12, 6 );
	container->set_pad( padding, padding, padding, padding );

	container->add( std::make_shared<gui::color_picker>(), 1.0 );
	container->add( std::make_shared<gui::color_picker>(), 1.0 );

	return container;
}
*/

////////////////////////////////////////

int safemain( int argc, char **argv )
{
	app = std::make_shared<gui::application>();
	app->push();
//	app->set_style( std::make_shared<gui::dark_style>() );

	auto win = app->new_window();
	win->set_title( app->active_platform() );

	auto label1 = std::make_shared<gui::label>( "Hello", base::alignment::TOP_LEFT );
	label1->set_color( gl::red );
	auto label2 = std::make_shared<gui::label>( "World", base::alignment::BOTTOM_RIGHT );
	label2->set_color( gl::green );
	auto button = std::make_shared<gui::button>( "Click Me" );
	button->set_expansion_flex( 1.0 );

	auto box = std::make_shared<gui::box>();
	box->set_padding( 12, 12, 5, 5 );
	box->set_spacing( 8, 0 );
	box->add( label1 );
	box->add( label2 );
	box->add( button );
	win->set_widget( box );

	/*
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
	*/
	/*
	else if ( test == "tree" )
		win->set_widget( build_tree( dir ) );
	else if ( test == "edit" )
		win->set_widget( build_edit( dir ) );
	else if ( test == "color" )
		win->set_widget( build_color( dir ) );
	else if ( test == "all" )
		win->set_widget( build_all( dir ) );
	else
		throw std::runtime_error( "unknown test" );
	*/

	win->show();
	int code = app->run();
	app->pop();
	app.reset();
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
		base::print_exception( std::cerr, e );
	}
	return ret;
}

////////////////////////////////////////

