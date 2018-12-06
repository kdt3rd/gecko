//
// Copyright (c) 2014-2017 Ian Godin
// SPDX-License-Identifier: MIT
//

#include <platform/platform.h>
#include <platform/system.h>
#include <platform/dispatcher.h>
#include <platform/simple_window.h>
#include <gl/opengl.h>
#include <gl/api.h>
#include <gl/mesh.h>
#include <gl/png_image.h>
#include <base/contract.h>
#include <base/timer.h>
#include <base/function_traits.h>
#include <draw/rectangle.h>
#include <layout/packing.h>
#include <layout/box.h>
#include <layout/tree.h>
#include <layout/form.h>
#include <layout/field.h>
#include <layout/grid.h>
#include <layout/scroll.h>

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

	void draw( platform::context &ctxt )
	{
		_rect.set_position( this->x(), this->y() );
		_rect.set_size( this->width(), this->height() );
		_rect.draw( ctxt );
//		ogl.save_matrix();
//		ogl.translate( this->x(), this->y() );
		for ( auto &c: _children )
			c( ctxt );
//		ogl.restore_matrix();
	}

	template<typename W>
	void draw_subchild( const std::shared_ptr<W> &a )
	{
		_children.push_back( [=]( platform::context &ctxt ) { a->draw( ctxt ); } );
	}

	template<typename W, typename ...Args>
	auto add_child( const std::shared_ptr<W> &a, Args &&...args ) -> decltype( Area::add( a, std::forward<Args>(args)...) )
	{
		draw_subchild( a );
		return Area::add( a, std::forward<Args>( args )... );
	}

private:
	draw::rectangle _rect;
	std::list<std::function<void(platform::context&)>> _children;
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
		_children.push_back( [=]( platform::context &ctxt ) { a->draw( ctxt ); } );
	}

	void draw( platform::context &ctxt )
	{
		_rect.set_position( this->x(), this->y() );
		_rect.set_size( this->width(), this->height() );
		_rect.draw( ctxt );
//		ogl.save_matrix();
//		ogl.translate( this->x(), this->y() );
		for ( auto &c: _children )
			c( ctxt );
//		ogl.restore_matrix();
	}

private:
	draw::rectangle _rect;
	std::list<std::function<void(platform::context&)>> _children;
};

typedef terminal_widget<layout::area> simple;

std::list<std::shared_ptr<layout::area>> keepers;

std::shared_ptr<simple> make_simple( const gl::color &c )
{
	auto a = std::make_shared<simple>( c );
	keepers.push_back( a );
	return a;
}

std::shared_ptr<widget<layout::tree>> make_tree( void )
{
	auto groove = std::make_shared<simple>( gl::color( 0.45F, 0.45F, 0.45F ) );
	keepers.push_back( groove );
	groove->set_minimum_width( 15 );

	auto title = std::make_shared<simple>( gl::blue );
	keepers.push_back( title );

	auto result = std::make_shared<widget<layout::tree>>( gl::grey, groove, title );
	result->set_indent( 15 );
	result->set_spacing( 1, 5 );
	result->draw_subchild( groove );
	result->draw_subchild( title );

	return result;
}

int safemain( int argc, char *argv[] )
{
	unused( argc );
	unused( argv );

	// Create a window
	auto sys = platform::platform::find_running();
	auto win = std::make_shared<platform::simple_window>( sys->new_window() );

	win->set_title( "Layout" );
	auto render_guard = win->hw_context().begin_render();
	gl::api &ogl = win->hw_context().api();
	//ogl.setup_debugging();

	// OpenGL information & initialization
	gl::matrix4 matrix;

	// Create "widgets"
	widget<layout::packing> root( gl::white );
	root.set_padding( 5, 5, 5, 5 );
	root.set_spacing( 5, 5 );

	auto tree = make_tree();
	auto form = std::make_shared<terminal_widget<layout::form>>( gl::color( 0.75, 0.75, 0.75 ), tree );
	form->set_padding( 5, 5, 5, 5 );
	form->draw_subchild( tree );

	for ( size_t i = 0; i < 3; ++i )
	{
		auto l = std::make_shared<simple>( gl::blue );
		auto e = std::make_shared<simple>( gl::green );
		auto f = std::make_shared<terminal_widget<layout::field>>( gl::grey, l, e );
		l->set_minimum_width( 200 );
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
	auto ftest = std::make_shared<terminal_widget<layout::field>>( gl::grey, label, field );
	ftest->set_spacing( 5, 5 );
	ftest->draw_subchild( label );
	ftest->draw_subchild( field );

	auto right = std::make_shared<widget<layout::box>>( gl::gray, base::alignment::BOTTOM );
	right->add_child( ftest );
	right->set_spacing( 5, 5 );

	auto top = std::make_shared<widget<layout::box>>( gl::grey );
	auto center = std::make_shared<widget<layout::box>>( gl::black, base::alignment::BOTTOM );
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

	auto grid = std::make_shared<widget<layout::grid>>( gl::black );
	grid->add_columns( 3 );
	grid->add_rows( 3 );
	grid->set_padding( 5, 5, 5, 5 );
	grid->set_spacing( 5, 5 );
	grid->add_child( make_simple( gl::blue ), 0, 0, 2, 1 );
	grid->add_child( make_simple( gl::blue ), 2, 0, 1, 2 );
	grid->add_child( make_simple( gl::blue ), 1, 2, 2, 1 );
	grid->add_child( make_simple( gl::blue ), 0, 1, 1, 2 );
	grid->add_child( make_simple( gl::red ), 1, 1, 1, 1 );
	center->add_child( grid );

	auto scroll = std::make_shared<terminal_widget<layout::scroll>>( gl::black );
	{
		auto m = std::make_shared<simple>( gl::grey );
		auto h = std::make_shared<simple>( gl::blue );
		auto v = std::make_shared<simple>( gl::blue );
		auto c = std::make_shared<simple>( gl::red );
		h->set_minimum( 15, 15 );
		v->set_minimum( 15, 15 );
		c->set_minimum( 15, 15 );
		scroll->set_main( m );
		scroll->set_hscroll( h );
		scroll->set_vscroll( v );
		scroll->set_corner( c );
		scroll->draw_subchild( m );
		scroll->draw_subchild( h );
		scroll->draw_subchild( v );
		scroll->draw_subchild( c );
		scroll->set_expansion_flex( 1.0 );
		scroll->set_padding( 5, 5, 5, 5 );
		scroll->set_spacing( 5, 5 );
	}
	center->add_child( scroll );

	root.compute_bounds();
	win->set_minimum_size( win->query_screen()->to_native_horiz( root.minimum_width() ),
	                       win->query_screen()->to_native_vert( root.minimum_height() ) );
	win->resize( win->query_screen()->to_native_horiz( root.minimum_width() ),
	             win->query_screen()->to_native_vert( root.minimum_height() ) );

	// Render function
	win->exposed = [&]( void )
	{
		root.set_size( win->width(), win->height() );
		root.compute_bounds();
		root.compute_layout();

		win->hw_context().viewport( 0, 0, win->width(), win->height() );
		ogl.set_projection( gl::matrix4::ortho( 0, static_cast<float>( win->width() ), 0, static_cast<float>( win->height() ) ) );
		ogl.clear();

		root.draw( win->hw_context() );
	};

	// Key to take a screenshot.
	win->key_pressed = [&]( platform::event_source &, platform::scancode c )
	{
		if ( c == platform::scancode::KEY_S )
		{
			auto r = win->hw_context().begin_render();
			gl::png_write( "/tmp/test.png", static_cast<size_t>( win->width() ), static_cast<size_t>( win->height() ), 3 );
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
