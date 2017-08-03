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
#include <gui/slider.h>
#include <base/contract.h>

namespace {

static constexpr double padding = 12;
static std::shared_ptr<gui::application> app;

////////////////////////////////////////

int safemain( int argc, char **argv )
{
	app = std::make_shared<gui::application>();
	app->push();
//	app->set_style( std::make_shared<gui::dark_style>() );

	auto win = app->new_window();
	win->set_title( app->active_platform() );

	auto label = std::make_shared<gui::label>( "Hello World", base::alignment::LEFT );
	auto button = std::make_shared<gui::button>( "Click Me" );
	auto slider = std::make_shared<gui::slider>();

	auto box = std::make_shared<gui::box>( base::alignment::BOTTOM );
	box->set_padding( 12, 12, 5, 5 );
	box->set_spacing( 8, 0 );
	box->add( label );
	box->add( button );
	box->add( slider );
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

