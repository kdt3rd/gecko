
#include "cocoa_style.h"
#include <draw/composite.h>
#include <draw/object.h>
#include <draw/polylines.h>
#include <core/contract.h>
#include "application.h"
#include <core/data_resource.h>
#include <shaders.h>

namespace
{
	auto resource = core::resource( shaders );

	std::shared_ptr<gl::program> make_program( gl::context &ctxt, const std::string &vname, const std::string &fname )
	{
		auto vshader = ctxt.new_shader( gl::shader::type::VERTEX, resource( vname ) );
		auto fshader = ctxt.new_shader( gl::shader::type::FRAGMENT, resource( fname ) );
		return ctxt.new_program( vshader, fshader );
	}
}

////////////////////////////////////////

namespace
{
core::color bg { 0.9294, 0.9294, 0.9294 };

core::color border1 { 0.6039, 0.6039, 0.6039 };
draw::gradient grad1
{
	{ 0.00, { 1.0000, 1.0000, 1.0000 } },
	{ 0.22, { 1.0000, 1.0000, 1.0000 } },
	{ 0.56, { 0.9255, 0.9255, 0.9255 } },
	{ 0.72, { 0.9255, 0.9255, 0.9255 } },
	{ 0.83, { 0.9294, 0.9294, 0.9294 } },
	{ 0.94, { 0.9373, 0.9373, 0.9373 } },
	{ 1.00, { 0.9490, 0.9490, 0.9490 } },
};

core::color border2 { 0.3059, 0.2863, 0.4314 };
draw::gradient grad2
{
	{ 0.00, { 0.7804, 0.8039, 0.8275 } },
	{ 0.06, { 0.6784, 0.7059, 0.7608 } },
	{ 0.50, { 0.5176, 0.5647, 0.6510 } },
	{ 0.56, { 0.4549, 0.5098, 0.6157 } },
	{ 1.00, { 0.7451, 0.7843, 0.8157 } },
};

}

////////////////////////////////////////

namespace gui
{

////////////////////////////////////////

cocoa_style::cocoa_style( void )
//   	const std::shared_ptr<draw::canvas> &c  )
{
//	_default_font = application::current()->get_font( "Lucida Grande", "Regular", 14.0 );
//	postcondition( bool(_default_font), "font for cocoa style not found" );

//	_default_bold_font = application::current()->get_font( "Lucida Grande", "Bold", 14.0 );
//	postcondition( bool(_default_bold_font), "font for cocoa bold style not found" );
}

////////////////////////////////////////

cocoa_style::~cocoa_style( void )
{
}

////////////////////////////////////////

std::shared_ptr<draw::font> cocoa_style::default_font( bool bold )
{
	if ( bold )
		return _default_bold_font;
	else
		return _default_font;
}

////////////////////////////////////////

void cocoa_style::background( const std::shared_ptr<draw::canvas> &c )
{
	c->clear_color( bg );
	c->clear();
}

////////////////////////////////////////

core::size cocoa_style::button_size( const core::size &content )
{
	core::size full( content );
	full.grow( 12, 6 );
	full.ceil();
	full.set_height( std::max( full.h(), 21.0 ) );
	return full;
}

////////////////////////////////////////

core::rect cocoa_style::button_content( const core::rect &full )
{
	core::rect content( full );
	content.shrink( 6, 6, 3, 3 );
	return content;
}

////////////////////////////////////////

void cocoa_style::button_frame( const std::shared_ptr<draw::canvas> &c, const core::rect &r, bool pressed )
{
	construct( c );
	auto obj = draw_button_frame( c, r );
	obj->draw( *c );
}

////////////////////////////////////////

void cocoa_style::line_edit_frame( const std::shared_ptr<draw::canvas> &c, const core::rect &r, bool focused )
{
	draw::paint paint;

	paint.set_stroke_color( border1 );
	paint.set_stroke_width( 1.0 );
	paint.set_fill_color( { 1, 1, 1 } );

	core::path path;
	path.rectangle( r.top_left(), r.bottom_right() );
//	c->draw_path( path, paint );
}

////////////////////////////////////////

double cocoa_style::slider_size( const core::rect &r )
{
	return r.radius();
}

////////////////////////////////////////

void cocoa_style::slider_groove( const std::shared_ptr<draw::canvas> &c, const core::rect &rect )
{
	double size = slider_size( rect );
	core::rect r = rect;
	r.trim( size, size, 0.0, 0.0 );
	double extra = r.height() - 7.0;
	if ( extra > 0.0 )
	{
		r.set_y( r.y() + extra/2.0 );
		r.set_height( 7.0 );
	}

	draw::paint p;
	p.set_stroke_color( border2 );
	p.set_stroke_width( 1.0 );
	p.set_fill_linear( r.top_left(), r.bottom_left(), grad2 );

	core::path rpath;
	rpath.rounded_rect( r.top_left(), r.bottom_right(), 2 );

//	c->draw_path( rpath, p );
}

////////////////////////////////////////

void cocoa_style::slider_button( const std::shared_ptr<draw::canvas> &c, const core::rect &r, bool pressed, double val )
{
	draw::paint p;

	if ( pressed )
	{
		p.set_stroke_color( border2 );
		p.set_stroke_width( 1.0 );
		p.set_fill_linear( r.top_left(), r.bottom_left(), grad2 );
	}
	else
	{
		p.set_stroke_color( border1 );
		p.set_stroke_width( 1.0 );
		p.set_fill_linear( r.top_left(), r.bottom_left(), grad1 );
	}

	core::point center = { r.x( val, slider_size( r ) ), r.y( 0.5 ) };

	core::path rpath;
	rpath.circle( center, r.radius() );

//	c->draw_path( rpath, p );
}

////////////////////////////////////////

void cocoa_style::construct( const std::shared_ptr<draw::canvas> &c )
{
	if ( !_constructed )
	{
		_grad1 = grad1.generate( *c );
		_grad2 = grad2.generate( *c );

		// Button drawing
		{
			draw::polylines p;
			{
				core::path tmp;
				tmp.rounded_rect( { 0.5, 0.5 }, 19, 19, 3 );
				tmp.replay( p );
			}

			auto fill = std::make_shared<draw::object>();
			auto fillprog = make_program( *c, "quadrant.vert", "linear_gradient.frag" );
			checkgl();
			{
				fillprog->set_uniform( "txt", 0 );
				fillprog->set_uniform( "origin", core::point( 0.5, 0.5 ) );
				fillprog->set_uniform( "dir", core::point( 0, 19 ) );
				fillprog->set_uniform( "center", core::point( 10, 10 ) );
				fillprog->set_uniform( "top_left", core::point( { 100, 100 } ) );
				fillprog->set_uniform( "quad_size", core::size( { 250, 21 } ) );
				checkgl();

				auto m = p.filled();
				fill->set_mesh( *c, m, fillprog->get_attribute_location( "position" ) );
				checkgl();

				fill->set_program( fillprog );
				checkgl();

				fill->add_texture( _grad1 );
				checkgl();
			}

			auto outline = std::make_shared<draw::object>();
			auto outprog = make_program( *c, "quadrant.vert", "single_color.frag" );
			{
				outprog->set_uniform( "color", border1 );
				outprog->set_uniform( "center", core::point( { 10, 10 } ) );
				checkgl();

				auto m = p.stroked( 1.0 ).filled();
				outline->set_mesh( *c, m, outprog->get_attribute_location( "position" ) );
				checkgl();

				outline->set_program( outprog );
				checkgl();
			}

			auto result = std::make_shared<draw::composite>();
			result->add( fill );
			result->add( outline );

			draw_button_frame = [=]( const std::shared_ptr<draw::canvas> &c, const core::rect &r )
			{
				core::point tl = r.top_left() - core::point( { 0.5, 0.5 } );

				c->use_program( fillprog );
				fillprog->set_uniform( "top_left", tl );
				fillprog->set_uniform( "quad_size", r.size() );

				c->use_program( outprog );
				outprog->set_uniform( "top_left", tl );
				outprog->set_uniform( "quad_size", r.size() );

				return result;
			};
		}

		_constructed = true;
	}
}

////////////////////////////////////////

}

