
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
	callback_invalidate( _text, _align, _color, _font );
}

////////////////////////////////////////

line_edit::line_edit( datum<std::string> &&l, datum<alignment> &&a, datum<core::color> &&c, shared_datum<draw::font> &&f )
	: _text( std::move( l ) ), _align( std::move( a ) ), _color( std::move( c ) ), _font( std::move( f ) )
{
	callback_invalidate( _text, _align, _color, _font );
}

////////////////////////////////////////

line_edit::~line_edit( void )
{
}

////////////////////////////////////////

void line_edit::paint( const std::shared_ptr<draw::canvas> &c )
{
	auto style = application::current_style();
	style->line_edit_frame( c, *this, false );

	const std::string &str = _text.value();

//	core::point p = c->align_text( _font.value(), str, *this, _align.value() );


	draw::paint paint;
	paint.set_fill_color( _color.value() );
//	c->draw_text( _font.value(), p, str, paint );

	draw::font_extents fex = _font.value()->extents();
	draw::text_extents tex = _font.value()->extents( str.substr( 0, _cursor ) );

	core::path path;
//	path.move_to( p );
	path.move_by( { tex.x_advance, fex.descent } );
	path.line_by( { 0, -fex.height } );

	draw::paint pen;
	pen.set_stroke_color( _color.value() );
//	c->draw_path( path, pen );
}

////////////////////////////////////////

void line_edit::compute_minimum( void )
{
	draw::font_extents fex = _font.value()->extents();
	draw::text_extents tex = _font.value()->extents( _text.value() );
	set_minimum( tex.x_advance + 12, std::max( 21.0, fex.height ) );
}

////////////////////////////////////////

bool line_edit::key_press( platform::scancode c )
{
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
			if ( _cursor < _text.value().size() )
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
			if ( _cursor < _text.value().size() )
			{
				_cursor = _text.value().size();
				invalidate();
			}
			break;

		case platform::scancode::KEY_BACKSPACE:
			_cursor = std::min( _cursor, _text.value().size() );
			if ( _cursor > 0 )
			{
				std::string tmp( _text.value() );
				tmp.erase( tmp.begin() + _cursor - 1 );
				_text = std::move( tmp );
				--_cursor;
				invalidate();
			}
			break;

		case platform::scancode::KEY_DELETE:
			_cursor = std::min( _cursor, _text.value().size() );
			if ( _cursor < _text.value().size() )
			{
				std::string tmp( _text.value() );
				tmp.erase( tmp.begin() + _cursor );
				_text = std::move( tmp );
				invalidate();
			}
			break;

		default:
			return false;
	}

	return true;
}

////////////////////////////////////////

bool line_edit::text_input( char32_t c )
{
	if ( utf::is_graphic( c ) )
	{
		std::string tmp( _text.value() );
		_cursor = std::min( _cursor, tmp.size() );

		std::insert_iterator<std::string> it( tmp, tmp.begin() + _cursor );
		_cursor += utf::convert_utf8( c, it );

		_text = std::move( tmp );
		invalidate();
		return true;
	}
	else
		std::cout << "NON GRAPHIC: " << c << std::endl;

	return false;
}

////////////////////////////////////////

}

