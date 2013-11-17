
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

namespace {

////////////////////////////////////////

////////////////////////////////////////

void build_button( const std::shared_ptr<gui::window> &win )
{
	auto container = std::make_shared<gui::container<gui::box_layout>>( direction::DOWN );
	container->set_spacing( 6, 6 );
	container->set_pad( 12.5, 12.5, 12.5, 12.5 );
	container->add( std::make_shared<gui::label>( "Hello World" ) );
	container->add( std::make_shared<gui::button>( "Click Me" ) );
	container->add( std::make_shared<gui::slider>() );

	win->set_widget( container );
//	gui::builder builder( win );
//	auto layout = builder.new_layout<layout::box_layout>( direction::DOWN );
//	layout->set_pad( 12.0, 12.0, 12.0, 12.0 );
//	layout->set_spacing( 12.0, 12.0 );

//	auto area = layout->new_area();
//	builder.make_button( area, "Button" );

}

////////////////////////////////////////

int safemain( int argc, char **argv )
{
	auto app = std::make_shared<gui::application>();
	app->push();
	app->set_style( std::make_shared<gui::cocoa_style>() );

//	auto win1 = app->new_window();
//	build_form_layout( win1 );
//	win1->show();

//	auto win2 = app->new_window();
//	build_box_layout( win2 );
//	win2->show();

//	auto win3 = app->new_window();
//	build_grid_layout( win3 );
//	win3->show();

//	auto win4 = app->new_window();
//	build_tree_layout( win4 );
//	win4->show();

	auto win5 = app->new_window();
	build_button( win5 );
	win5->show();

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

