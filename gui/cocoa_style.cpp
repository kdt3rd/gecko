
#include "cocoa_style.h"
#include <draw/composite.h>
#include <draw/object.h>
#include <draw/stretchable.h>
#include <draw/polylines.h>
#include <core/contract.h>
#include "application.h"
#include <draw/shaders.h>

namespace
{

core::color bg { 0.9294, 0.9294, 0.9294 };

core::color border1 { 0.6039, 0.6039, 0.6039 };
core::gradient grad1
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
core::gradient grad2
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
	_default_font = application::current()->get_font( "Lucida Grande", "Regular", 14.0 );
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
	if ( pressed )
	{
		_button_frame_down->set( c, r );
		_button_frame_down->draw( *c );
	}
	else
	{
		_button_frame->set( c, r );
		_button_frame->draw( *c );
	}
}

////////////////////////////////////////

void cocoa_style::line_edit_frame( const std::shared_ptr<draw::canvas> &c, const core::rect &r, bool focused )
{
	core::paint paint;

	paint.set_stroke_color( border1 );
	paint.set_stroke_width( 1.0 );
	paint.set_fill_color( { 1, 1, 1 } );

	core::path path;
	path.rectangle( r.top_left(), r.bottom_right() );
//	c->draw_path( path, paint );
}

////////////////////////////////////////

double cocoa_style::slider_size( const core::rect &rect )
{
	return rect.radius();
}

////////////////////////////////////////

void cocoa_style::slider_groove( const std::shared_ptr<draw::canvas> &c, const core::rect &rect )
{
	construct( c );

	double rad = slider_size( rect );
	double h = rect.height() - 7;
	core::rect tmp( rect );
	tmp.trim( rad, rad, h/2, h/2 );

	_slider_groove->set( c, tmp );
	_slider_groove->draw( *c );
}

////////////////////////////////////////

void cocoa_style::slider_button( const std::shared_ptr<draw::canvas> &c, const core::rect &r, bool pressed, double val )
{
	construct( c );

	double rad = r.radius();
	core::rect tmp( rad * 2, rad * 2 );
	tmp.set_center( { r.x( val, rad ), r.y( val, rad ) } );

	_slider_button->set( c, tmp );
	_slider_button->draw( *c );
}

////////////////////////////////////////

void cocoa_style::construct( const std::shared_ptr<draw::canvas> &c )
{
	if ( !_constructed )
	{
		_grad1 = c->gradient( grad1 );
		_grad2 = c->gradient( grad2 );

		// Button drawing
		_button_frame = std::make_shared<draw::stretchable>();
		_button_frame_down = std::make_shared<draw::stretchable>();
		{
			core::path path;
			path.rounded_rect( { 0, 0 }, 20, 20, 3 );

			core::paint paint( border1 );
			paint.set_fill_linear( { 0, 0 }, { 0, 20 }, grad1 );

			_button_frame->create( c, path, paint, { 10, 10 } );

			paint.set_stroke_color( border2 );
			paint.set_fill_linear( { 0, 0 }, { 0, 20 }, grad2 );
			_button_frame_down->create( c, path, paint, { 10, 10 } );
		}

		// Slider groove
		_slider_groove = std::make_shared<draw::stretchable>();
		{
			core::path path;
			path.rounded_rect( { 0, 0 }, { 20, 7 }, 2 );

			core::paint paint( border2 );
			paint.set_fill_linear( { 0, 0 }, { 0, 7 }, grad2 );

			_slider_groove->create( c, path, paint, { 10, 3.5 } );
		}

		// Slider button drawing
		_slider_button = std::make_shared<draw::stretchable>();
		{
			core::path path;
			path.circle( { 10, 10 }, 9 );

			core::paint paint( border1 );
			paint.set_fill_linear( { 0, 0 }, { 0, 20 }, grad1 );

			_slider_button->create( c, path, paint, { 10, 10 } );
		}

		_constructed = true;
	}
}

////////////////////////////////////////

}

