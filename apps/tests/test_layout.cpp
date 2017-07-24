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
#include <layout/field_layout.h>
#include <layout/grid_layout.h>

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

template<typename Area>
class terminal_widget : public Area
{
public:
	terminal_widget( const gl::color &c )
		: _rect( c )
	{
		this->set_minimum( 50, 25 );
	}

	template<typename ...Args>
	terminal_widget( const gl::color &c, Args &&...args )
		: Area( std::forward<Args>( args )... ), _rect( c )
	{
		this->set_minimum( 100, 25 );
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

typedef terminal_widget<layout::area> simple;

std::list<std::shared_ptr<layout::area>> keepers;

std::shared_ptr<simple> make_simple( const gl::color &c )
{
	auto a = std::make_shared<simple>( c );
	keepers.push_back( a );
	return a;
}

std::shared_ptr<widget<layout::tree_layout>> make_tree( void )
{
	auto groove = std::make_shared<simple>( gl::color( 0.45, 0.45, 0.45 ) );
	keepers.push_back( groove );
	groove->set_minimum_width( 10 );

	auto title = std::make_shared<simple>( gl::blue );
	keepers.push_back( title );

	auto result = std::make_shared<widget<layout::tree_layout>>( gl::grey, groove, title );
	result->set_spacing( 5, 5 );
	result->draw_subchild( groove );
	result->draw_subchild( title );

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
	widget<layout::packing_layout> root( gl::white );
	root.set_padding( 5, 5, 5, 5 );
	root.set_spacing( 5, 5 );

	auto tree = make_tree();
	auto form = std::make_shared<terminal_widget<layout::form_layout>>( gl::magenta, tree );
	form->draw_subchild( tree );

	for ( size_t i = 0; i < 3; ++i )
	{
		auto l = std::make_shared<simple>( gl::blue );
		auto e = std::make_shared<simple>( gl::green );
		auto f = std::make_shared<terminal_widget<layout::field_layout>>( gl::grey, l, e );
		e->set_minimum_width( 150 );
		f->set_spacing( 5, 5 );
		f->draw_subchild( l );
		f->draw_subchild( e );
		form->add( f );

		auto c = make_tree();
		c->add_child( f );
		auto tmp = make_tree();
		tmp->add_child( make_simple( gl::blue ) );
		c->add_child( tmp );
		for ( size_t j = 0; j <= i; ++j )
		{
			auto tmp = make_tree();
			tmp->add_child( c );
			c = tmp;
		}
		tree->add_child( c );
	}

	auto label = std::make_shared<simple>( gl::red );
	auto field = std::make_shared<simple>( gl::red );
	auto ftest = std::make_shared<terminal_widget<layout::field_layout>>( gl::grey, label, field );
	ftest->set_spacing( 5, 5 );
	ftest->draw_subchild( label );
	ftest->draw_subchild( field );

	auto right = std::make_shared<widget<layout::box_layout>>( gl::gray, base::alignment::BOTTOM );
	right->add_child( ftest );
	right->set_spacing( 5, 5 );

	auto top = std::make_shared<widget<layout::box_layout>>( gl::grey );
	auto center = std::make_shared<widget<layout::grid_layout>>( gl::black );
	root.add_child( top, base::alignment::TOP );
	root.add_child( form, base::alignment::LEFT );
	root.add_child( right, base::alignment::RIGHT );
	root.add_child( center, base::alignment::CENTER );

	top->set_padding( 5, 5, 5, 5 );
	top->set_spacing( 5, 5 );
	for ( size_t i = 0; i < 10; ++i )
	{
		auto a = std::make_shared<simple>( gl::yellow );
		a->set_expansion_flex( 1.0 );
		top->add_child( a );
	}

	center->add_columns( 3 );
	center->add_rows( 3 );
	center->set_padding( 5, 5, 5, 5 );
	center->set_spacing( 5, 5 );
	center->add_child( make_simple( gl::blue ), 0, 0, 2, 1 );
	center->add_child( make_simple( gl::blue ), 2, 0, 1, 2 );
	center->add_child( make_simple( gl::blue ), 1, 2, 2, 1 );
	center->add_child( make_simple( gl::blue ), 0, 1, 1, 2 );
	center->add_child( make_simple( gl::red ), 1, 1, 1, 1 );

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

