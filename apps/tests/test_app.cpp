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
#include <gui/checkbox.h>
#include <gui/radio_button.h>
#include <gui/line_edit.h>
#include <gui/scroll_bar.h>
#include <base/contract.h>
#include <draw/icons.h>

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

	win->in_context( [&]( void )
	{
		auto label = std::make_shared<gui::label>( "Hello World", base::alignment::LEFT );
		auto button = std::make_shared<gui::button>( "Click Me" );
		auto slider = std::make_shared<gui::slider>();
		auto cbox = std::make_shared<gui::checkbox>();
		auto rbutton = std::make_shared<gui::radio_button>();
		auto ledit = std::make_shared<gui::line_edit>();
		auto sbar = std::make_shared<gui::scroll_bar>();

		auto box = std::make_shared<gui::box>( base::alignment::BOTTOM );
		box->set_padding( 12, 12, 5, 5 );
		box->set_spacing( 8, 2 );
		box->add( label );
		box->add( button );
		box->add( slider );
		box->add( cbox );
		box->add( rbutton );
		box->add( ledit );
		box->add( sbar );

		win->set_widget( box );
	} );

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

