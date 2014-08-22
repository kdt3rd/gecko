
#include <iostream>
#include <sstream>
#include <random>
#include <functional>
#include <memory>

#include <gui/application.h>
#include <viewer/viewer.h>

//constexpr double padding = 12;
std::shared_ptr<gui::application> app;

namespace {

////////////////////////////////////////

int safemain( int argc, char **argv )
{
	app = std::make_shared<gui::application>();
	app->push();

	auto win = app->new_window();
	win->set_title( app->active_platform() );

	auto viewer = std::make_shared<viewer::viewer>();
	win->set_widget( viewer );

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

