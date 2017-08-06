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
#include <gui/choices.h>
#include <gui/line_edit.h>
#include <gui/scroll_bar.h>
#include <gui/scroll_area.h>
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

	auto win = app->new_window();
	win->set_title( app->active_platform() );

	win->in_context( [&]( void )
	{
		auto label = std::make_shared<gui::label>( "Hello World", base::alignment::LEFT );
		auto button = std::make_shared<gui::button>( "Click Me" );
		auto slider = std::make_shared<gui::slider>();
		auto cbox = std::make_shared<gui::checkbox>();
		auto choices = std::make_shared<gui::choices>();
		choices->add_choice( "Choice 1" );
		choices->add_choice( "Choice 2" );
		choices->add_choice( "Choice 3" );
		auto ledit = std::make_shared<gui::line_edit>();
		auto sbar = std::make_shared<gui::scroll_bar>();
		auto sarea = std::make_shared<gui::scroll_area>();
		auto center = std::make_shared<gui::label>( "Scroll!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!END", base::alignment::CENTER );
		sarea->set_widget( center );
		sarea->layout_target()->set_expansion_flex( 1.0 );

		auto box = std::make_shared<gui::box>( base::alignment::BOTTOM );
		box->set_padding( 12, 12, 5, 5 );
		box->set_spacing( 8, 2 );
		box->add( label );
		size_t n = box->add( button );
		button->when_activated.connect( [=]( void ) { box->remove( n ); box->remove( n + 1 ); } );
		box->add( slider );
		box->add( cbox );
		box->add( choices );
		box->add( ledit );
		box->add( sbar );
		box->add( sarea );

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

