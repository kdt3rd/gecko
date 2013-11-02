
#include <layout/form_layout.h>
#include <gui/application.h>
#include <gui/cocoa_style.h>

namespace {

////////////////////////////////////////

void build_window( const std::shared_ptr<gui::window> &win )
{
	using layout::form_layout;

	gui::builder builder( win );
	auto layout = builder.new_layout<form_layout>( layout::direction::RIGHT );

	auto row = layout->new_row();
	builder.make_label( row.first, "Hello World" );
	builder.make_button( row.second, "Press Me" );

	row = layout->new_row();
	builder.make_label( row.first, "Hello World" );
	builder.make_button( row.second, "Hello World" );
}

////////////////////////////////////////

int safemain( int argc, char **argv )
{
	auto app = std::make_shared<gui::application>();
	app->push();
	app->set_style( std::make_shared<gui::cocoa_style>() );

	auto win = app->new_window();
	build_window( win );
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

