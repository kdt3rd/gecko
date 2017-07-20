//
// Copyright (c) 2014-2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <platform/platform.h>
#include <platform/system.h>
#include <platform/dispatcher.h>
#include <gl/opengl.h>
#include <gl/api.h>
#include <gl/mesh.h>
#include <gl/png_image.h>
#include <base/contract.h>
#include <base/timer.h>
#include <base/math_functions.h>
#include <draw/path.h>
#include <draw/rectangle.h>
#include <layout/packing_layout.h>
#include <layout/box_layout.h>
#include <layout/tree_layout.h>

namespace
{

int safemain( int /*argc*/, char * /*argv*/ [] )
{
	std::vector<gl::color> colors = { gl::red, gl::green, gl::blue, gl::white, gl::cyan, gl::cyan, gl::yellow, gl::color( 0.5, 0.5, 0.5 ) };

	std::vector<std::shared_ptr<layout::area>> widgets( 7 );
	widgets[0] = std::make_shared<layout::area>();
	widgets[1] = std::make_shared<layout::area>();
	widgets[2] = std::make_shared<layout::area>();
	widgets[3] = std::make_shared<layout::area>();
	widgets[4] = std::make_shared<layout::area>();
	widgets[5] = std::make_shared<layout::area>();
	widgets[6] = std::make_shared<layout::area>();

	widgets[0]->set_minimum( 50, 25 );
	widgets[1]->set_minimum( 50, 50 );
	widgets[2]->set_minimum( 50, 50 );
	widgets[3]->set_minimum( 50, 50 );
	widgets[4]->set_minimum( 10, 10 );
	widgets[5]->set_minimum( 50, 35 );
	widgets[6]->set_minimum( 50, 35 );

	for ( auto &w: widgets )
		w->set_expansion_flex( 1.0 );

	/*
	auto subl = std::make_shared<layout::box_layout>( base::alignment::BOTTOM );
	subl->set_padding( 5, 5, 5, 5 );
	subl->set_spacing( 5, 5 );
	subl->add( widgets[4] );
	subl->add( widgets[5] );
	subl->add( widgets[6] );
	*/

	auto subl = std::make_shared<layout::tree_layout>( widgets[4], widgets[5], widgets[6] );
	subl->set_padding( 5, 5, 5, 5 );
	subl->set_spacing( 5, 5 );

	// Setup constraints for the widgets
	layout::packing_layout lay;
	lay.set_padding( 15, 15, 10, 10 );
	lay.set_spacing( 5, 3 );
	lay.add( widgets[1], base::alignment::TOP );
	lay.add( widgets[0], base::alignment::BOTTOM );
	lay.add( subl, base::alignment::LEFT );
	lay.add( widgets[2], base::alignment::RIGHT );
	lay.add( widgets[3], base::alignment::CENTER );
	lay.compute_bounds();

	// Create a window
	auto sys = platform::platform::find_running();
	auto win = sys->new_window();
	win->resize( 400, 400 );
	win->set_title( "Layout" );
	win->acquire();

	// OpenGL information & initialization
	gl::matrix4 matrix;
	gl::api ogl;
	//ogl.setup_debugging();

	// Create rectangles for each widget
	std::vector<draw::rectangle> rects;
	for ( size_t i = 0; i < widgets.size(); ++i )
		rects.emplace_back( colors[i] );
	rects.emplace_back( colors.back() );

	// Render function
	win->exposed = [&]( void )
	{
		win->acquire();

		lay.set_size( win->width(), win->height() );
		lay.compute_layout();

		matrix = gl::matrix4::ortho( 0, static_cast<float>( win->width() ), 0, static_cast<float>( win->height() ) );

		ogl.clear();
		ogl.viewport( 0, 0, win->width(), win->height() );
		ogl.save_matrix();

		for ( size_t i = 0; i < widgets.size(); ++i )
		{
			if ( i == 4 )
			{
				auto &r = rects.back();
				r.resize( subl->x1(), subl->y1(), subl->width(), subl->height() );
				r.draw( ogl, matrix );
				ogl.translate( subl->x1(), subl->y1() );
			}
			auto &w = *widgets[i];
			auto &r = rects[i];
			r.resize( w.x1(), w.y1(), w.width(), w.height() );
			r.draw( ogl, matrix );
		}

		ogl.restore_matrix();
		win->release();

		// Cause a redraw to continue the animation
		//win->invalidate( base::rect() );
	};

	// Key to take a screenshot.
	win->key_pressed = [&]( const std::shared_ptr<platform::keyboard> &, platform::scancode c )
	{
		if ( c == platform::scancode::KEY_S )
		{
			win->acquire();
			gl::png_write( "/tmp/test.png", static_cast<size_t>( win->width() ), static_cast<size_t>( win->height() ), 3 );
			win->release();
		}
	};

	win->show();

	auto dispatch = sys->get_dispatcher();
	return dispatch->execute();
}

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

