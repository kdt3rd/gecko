//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <iostream>
#include "line_edit.h"
#include <utf/utf.h>

namespace gui
{

////////////////////////////////////////

line_edit::line_edit( void )
{
	_prompt.set_text( "Type here" );
}

////////////////////////////////////////

line_edit::line_edit( std::string l )
	: _text( l )
{
}

////////////////////////////////////////

line_edit::~line_edit( void )
{
}

////////////////////////////////////////

void line_edit::build( gl::api &ogl )
{
	const style &s = context::current().get_style();
	const auto &f = s.body_font();

	script::font_extents fex = f->extents();
	script::text_extents tex = f->extents( _text.get_text() );
	layout_target()->set_minimum( tex.x_advance + 4, fex.height - fex.descent + 2  );

	_text.set_font( f );
	_text.set_color( s.primary_text( s.background_color() ) );
	_prompt.set_font( f );
	_prompt.set_color( s.disabled_text( s.background_color() ) );
	_line.set_color( s.dominant_color() );
	_marker.set_color( s.dominant_color() );
}

////////////////////////////////////////

void line_edit::paint( gl::api &ogl )
{
	script::font_extents fex = _text.get_font()->extents();
	_line.set_position( x(), y() + height() - 1.F );
	_line.set_size( width(), 1.5F );
	_text.set_position( x() + 2.F, y() + height() + fex.descent );
	_prompt.set_position( x() + 2.F, y() + height() + fex.descent );

	_line.draw( ogl );
	if ( _text.get_text().empty() )
		_prompt.draw( ogl );
	else
		_text.draw( ogl );

	const std::string &str = _text.get_text();
	script::text_extents tex = _text.get_font()->extents( str.substr( 0, _cursor ) );

	_marker.set_position( x() + 2.F + tex.x_advance - 0.5F, y() + height() + fex.descent - fex.ascent );
	_marker.set_size( 1.5F, fex.ascent - fex.descent );
	_marker.draw( ogl );
}

////////////////////////////////////////

bool line_edit::key_press( platform::scancode c )
{
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
			if ( _cursor < _text.get_text().size() )
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
			if ( _cursor < _text.get_text().size() )
			{
				_cursor = _text.get_text().size();
				invalidate();
			}
			break;

		case platform::scancode::KEY_BACKSPACE:
			_cursor = std::min( _cursor, _text.get_text().size() );
			if ( _cursor > 0 )
			{
				std::string tmp( _text.get_text() );
				tmp.erase( tmp.begin() + long(_cursor) - 1 );
				_text.set_text( tmp );
				--_cursor;
				invalidate();
			}
			break;

		case platform::scancode::KEY_DELETE:
			_cursor = std::min( _cursor, _text.get_text().size() );
			if ( _cursor < _text.get_text().size() )
			{
				std::string tmp( _text.get_text() );
				tmp.erase( tmp.begin() + long(_cursor) );
				_text.set_text( tmp );
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
		std::string tmp( _text.get_text() );
		_cursor = std::min( _cursor, tmp.size() );

		std::insert_iterator<std::string> it( tmp, tmp.begin() + long(_cursor) );
		_cursor += utf::convert_utf8( c, it );

		_text.set_text( tmp );
		invalidate();
		return true;
	}
	else
		std::cout << "NON GRAPHIC: " << static_cast<uint32_t>(c) << std::endl;

	return false;
}

////////////////////////////////////////

}

