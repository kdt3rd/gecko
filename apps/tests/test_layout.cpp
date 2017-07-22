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
#include <base/function_traits.h>
#include <draw/path.h>
#include <draw/rectangle.h>
#include <layout/packing_layout.h>
#include <layout/box_layout.h>
#include <layout/tree_layout.h>
#include <layout/form_layout.h>
#include <layout/label_layout.h>

namespace
{

template<typename Area>
class widget : public Area
{
public:
	widget( const gl::color &c )
		: _rect( c )
	{
		this->set_minimum( 100, 25 );
	}

	template<typename ...Args>
	widget( const gl::color &c, Args &&...args )
		: Area( std::forward<Args>( args )... ), _rect( c )
	{
		this->set_minimum( 100, 25 );
	}

	void draw( gl::api &ogl, const gl::matrix4 &m )
	{
		_rect.resize( this->x(), this->y(), this->width(), this->height() );
		_rect.draw( ogl, m );
		ogl.save_matrix();
		ogl.translate( this->x(), this->y() );
		for ( auto &c: _children )
			c( ogl, m );
		ogl.restore_matrix();
	}

	template<typename W>
	void draw_subchild( const std::shared_ptr<W> &a )
	{
		_children.push_back( [=]( gl::api &ogl, const gl::matrix4 &m ) { a->draw( ogl, m ); } );
	}

	template<typename W, typename ...Args>
	auto add_child( const std::shared_ptr<W> &a, Args &&...args ) -> decltype( Area::add( a, std::forward<Args>(args)...) )
	{
		draw_subchild( a );
		return Area::add( a, std::forward<Args>( args )... );
	}

private:
	draw::rectangle _rect;
	std::list<std::function<void(gl::api&,const gl::matrix4&)>> _children;
};

template<>
class widget<layout::area> : public layout::area
{
public:
	widget( const gl::color &c )
		: _rect( c )
	{
		this->set_minimum( 50, 25 );
	}

	void draw( gl::api &ogl, const gl::matrix4 &m )
	{
		_rect.resize( this->x(), this->y(), this->width(), this->height() );
		_rect.draw( ogl, m );
	}

private:
	draw::rectangle _rect;
};

template<>
class widget<layout::tree_layout> : public layout::tree_layout
{
public:
	widget( const gl::color &cl, const std::shared_ptr<layout::area> &g, const std::shared_ptr<layout::area> &t, const std::shared_ptr<layout::area> &c )
		: tree_layout( g, t, c ), _rect( cl )
	{
	}

	template<typename W>
	void draw_subchild( const std::shared_ptr<W> &a )
	{
		_children.push_back( [=]( gl::api &ogl, const gl::matrix4 &m ) { a->draw( ogl, m ); } );
	}

	void draw( gl::api &ogl, const gl::matrix4 &m )
	{
		_rect.resize( this->x(), this->y(), this->width(), this->height() );
		_rect.draw( ogl, m );
		ogl.save_matrix();
		ogl.translate( this->x(), this->y() );
		for ( auto &c: _children )
			c( ogl, m );
		ogl.restore_matrix();
	}

private:
	draw::rectangle _rect;
	std::list<std::function<void(gl::api&,const gl::matrix4&)>> _children;
};

typedef widget<layout::area> simple;

std::list<std::shared_ptr<layout::area>> keepers;

template<typename W>
std::shared_ptr<widget<layout::tree_layout>> make_tree( const std::shared_ptr<W> &content )
{
	auto groove = std::make_shared<simple>( gl::blue );
	groove->set_minimum_width( 10 );
	auto title = std::make_shared<simple>( gl::blue );
	auto result = std::make_shared<widget<layout::tree_layout>>( gl::grey, groove, title, content );
	result->draw_subchild( groove );
	result->draw_subchild( title );
	result->draw_subchild( content );
	result->set_spacing( 5, 5 );
	keepers.push_back( groove );
	keepers.push_back( title );
	keepers.push_back( content );
	keepers.push_back( result );
	return result;
}

int safemain( int /*argc*/, char * /*argv*/ [] )
{

	// Create a window
	auto sys = platform::platform::find_running();
	auto win = sys->new_window();
	win->set_title( "Layout" );
	win->acquire();

	// OpenGL information & initialization
	gl::matrix4 matrix;
	gl::api ogl;
	//ogl.setup_debugging();

	// Create "widgets"
	widget<layout::packing_layout> root( gl::grey );
	root.set_padding( 5, 5, 5, 5 );
	root.set_spacing( 5, 5 );

	auto list = std::make_shared<widget<layout::box_layout>>( gl::black, base::alignment::BOTTOM );
	list->set_padding( 5, 5, 5, 5 );
	list->set_spacing( 5, 5 );
	for ( size_t i = 0; i < 5; ++i )
	{
		auto t = std::make_shared<simple>( gl::white );
		t->set_minimum_width( 200 );
		auto c = make_tree( t );
		for ( size_t j = 0; j < i; ++j )
			c = make_tree( c );
		list->add_child( make_tree( c ) );
	}

	auto top = std::make_shared<widget<layout::box_layout>>( gl::green );
	root.add_child( top, base::alignment::TOP );
	root.add_child( list, base::alignment::LEFT );
	root.add_child( std::make_shared<simple>( gl::blue ), base::alignment::RIGHT );

	top->set_padding( 5, 5, 5, 5 );
	top->set_spacing( 5, 5 );
	for ( size_t i = 0; i < 10; ++i )
	{
		auto a = std::make_shared<simple>( gl::yellow );
		a->set_expansion_flex( 1.0 );
		top->add_child( a );
	}

	root.compute_bounds();
	win->resize( root.minimum_width(), root.minimum_height() );

	// Render function
	win->exposed = [&]( void )
	{
		win->acquire();

		root.set_size( win->width(), win->height() );
		root.compute_bounds();
		root.compute_layout();

		matrix = gl::matrix4::ortho( 0, static_cast<float>( win->width() ), 0, static_cast<float>( win->height() ) );

		ogl.clear();
		ogl.viewport( 0, 0, win->width(), win->height() );

		root.draw( ogl, matrix );

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

