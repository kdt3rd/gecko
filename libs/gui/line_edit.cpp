
#include <iostream>
#include "line_edit.h"
#include "application.h"
#include "style.h"
#include <utf/utf.h>

namespace gui
{

////////////////////////////////////////

line_edit::line_edit( void )
{
}

////////////////////////////////////////

line_edit::line_edit( std::string l, base::alignment a, const base::color &c, const std::shared_ptr<script::font> &f )
	: _text( std::move( l ) ), _align( a ), _color( c ), _font( f )
{
}

////////////////////////////////////////

line_edit::~line_edit( void )
{
}

////////////////////////////////////////

void line_edit::paint( const std::shared_ptr<draw::canvas> &c )
{
	if ( !_frame )
	{
		_frame = std::make_shared<draw::stretchable>();
		base::paint paint( { 0.27, 0.27, 0.27 } );
		paint.set_fill_color( { 1, 1, 1 } );

		base::path path;
		path.rectangle( { 0, 0 }, 20, 20 );

		_frame->create( c, path, paint, { 10, 10 } );
	}

	_frame->set( c, *this );
	_frame->draw( *c );

	const std::string &str = _text;
	base::point p = c->align_text( _font, str, *this, _align );

	base::paint paint;
	paint.set_fill_color( _color );
	c->draw_text( _font, p, str, paint );

	script::font_extents fex = _font->extents();
	script::text_extents tex = _font->extents( str.substr( 0, _cursor ) );

	if ( !_marker )
	{
		_marker = std::make_shared<draw::stretchable>();
		base::path path;
		path.move_to( { 0.5, 0 } );
		path.line_by( { 0, 10 } );

		base::paint pen( { 0, 0, 0 }, 1.0 );

		_marker->create( c, path, pen, { 0.5, 5 } );
	}
	double h = fex.ascent - fex.descent;
	p = p + base::point( tex.x_advance, -fex.descent );
	base::rect tmp( p - base::point( 0, h ), 2, h );

	_marker->set( c, tmp );
	_marker->draw( *c );
}

////////////////////////////////////////

void line_edit::compute_minimum( void )
{
	script::font_extents fex = _font->extents();
	script::text_extents tex = _font->extents( _text );
	set_minimum( tex.x_advance + 12, std::max( 21.0, fex.height ) );
}

////////////////////////////////////////

bool line_edit::key_press( platform::scancode c )
{
	std::cout << "Key press!" << std::endl;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
	switch ( c )
	{
		case platform::scancode::KEY_LEFT:
			if ( _cursor > 0 )
			{
				--_cursor;
				invalidate();
			}
			break;

		case platform::scancode::KEY_RIGHT:
			if ( _cursor < _text.size() )
			{
				++_cursor;
				invalidate();
			}
			break;

		case platform::scancode::KEY_HOME:
			if ( _cursor > 0 )
			{
				_cursor = 0;
				invalidate();
			}
			break;

		case platform::scancode::KEY_END:
			if ( _cursor < _text.size() )
			{
				_cursor = _text.size();
				invalidate();
			}
			break;

		case platform::scancode::KEY_BACKSPACE:
			_cursor = std::min( _cursor, _text.size() );
			if ( _cursor > 0 )
			{
				std::string tmp( _text );
				tmp.erase( tmp.begin() + long(_cursor) - 1 );
				_text = std::move( tmp );
				--_cursor;
				invalidate();
			}
			break;

		case platform::scancode::KEY_DELETE:
			_cursor = std::min( _cursor, _text.size() );
			if ( _cursor < _text.size() )
			{
				std::string tmp( _text );
				tmp.erase( tmp.begin() + long(_cursor) );
				_text = std::move( tmp );
				invalidate();
			}
			break;

		default:
			return false;
	}
#pragma GCC diagnostic pop

	return true;
}

////////////////////////////////////////

bool line_edit::text_input( char32_t c )
{
	if ( utf::is_graphic( c ) )
	{
		std::string tmp( _text );
		_cursor = std::min( _cursor, tmp.size() );

		std::insert_iterator<std::string> it( tmp, tmp.begin() + long(_cursor) );
		_cursor += utf::convert_utf8( c, it );

		_text = std::move( tmp );
		invalidate();
		return true;
	}
	else
		std::cout << "NON GRAPHIC: " << static_cast<uint32_t>(c) << std::endl;

	return false;
}

////////////////////////////////////////

}

