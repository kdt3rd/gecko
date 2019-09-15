// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "line_edit.h"

#include "application.h"

#include <iostream>
#include <platform/system.h>
#include <utf/utf.h>

namespace gui
{
////////////////////////////////////////

line_edit_w::line_edit_w( void ) { _prompt.set_text( "Type here" ); }

////////////////////////////////////////

line_edit_w::line_edit_w( std::string l ) : _text( l ) {}

////////////////////////////////////////

line_edit_w::~line_edit_w( void ) {}

////////////////////////////////////////

void line_edit_w::build( context &ctxt )
{
    const style &s = ctxt.get_style();
    const auto & f = s.body_font();

    script::font_extents fex = f->extents();
    const coord          ht  = coord( fex.height ) + ctxt.from_native_vert( 2 );
    const coord          wt  = coord( fex.width ) + ctxt.from_native_vert( 4 );
    layout_target()->set_minimum( wt, ht );

    _text.set_font( f );
    _text.set_color( s.primary_text( s.background_color() ) );
    _prompt.set_font( f );
    _prompt.set_color( s.disabled_text( s.background_color() ) );
    _line.set_color( s.dominant_color() );
    _marker.set_color( s.dominant_color() );
}

////////////////////////////////////////

void line_edit_w::paint( context &ctxt )
{
    script::font_extents fex = _text.get_font()->extents();

    auto voff = coord( fex.descent );
    auto inset =
        ctxt.from_native_horiz( static_cast<platform::coord_type>( 2 ) );

    _line.set_position( x(), y() + height() - inset / 2.F );
    _line.set_size( width(), inset * 0.75F );

    _text.set_position( x() + inset, y() + height() + voff );
    _prompt.set_position( x() + inset, y() + height() + voff );

    platform::context &hwc = ctxt.hw_context();
    _line.draw( hwc );
    if ( _text.get_text().empty() )
        _prompt.draw( hwc );
    else
        _text.draw( hwc );

    const std::string &  str = _text.get_text();
    script::text_extents tex =
        _text.get_font()->extents( str.substr( 0, _cursor ) );

    coord mpos = tex.width.as<coord>();
    coord moff = fex.ascent.as<coord>();
    _marker.set_position( x() + inset + mpos, y() + height() + voff - moff );
    _marker.set_size( inset * 0.75F, moff - voff );
    _marker.draw( hwc );
}

////////////////////////////////////////

bool line_edit_w::key_press( const event &e )
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
    platform::scancode c = e.raw_key().keys[0];
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
                tmp.erase( tmp.begin() + long( _cursor ) - 1 );
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
                tmp.erase( tmp.begin() + long( _cursor ) );
                _text.set_text( tmp );
                invalidate();
            }
            break;

        default: return false;
    }
#pragma GCC diagnostic pop

    return true;
}

////////////////////////////////////////

bool line_edit_w::text_input( const event &e )
{
    char32_t c = e.raw_text().text;
    if ( utf::is_graphic( c ) )
    {
        std::string tmp( _text.get_text() );
        _cursor = std::min( _cursor, tmp.size() );

        std::insert_iterator<std::string> it(
            tmp, tmp.begin() + long( _cursor ) );
        _cursor += utf::convert_utf8( c, it );

        _text.set_text( tmp );
        invalidate();
        return true;
    }
    else
        std::cout << "NON GRAPHIC: " << static_cast<uint32_t>( c ) << std::endl;

    return false;
}

////////////////////////////////////////

bool line_edit_w::mouse_press( const event &e )
{
    int button = e.raw_mouse().button;
    if ( button == 1 )
    {
        // TODO: set cursor position
        context::current().set_focus( shared_from_this() );
    }
    else if ( button == 2 )
    {
        auto paste = application::current()->get_system()->query_selection(
            platform::selection_type::MOUSE );
        if ( !paste.second.empty() )
        {
            std::cout << "recv paste: " << paste.first.size()
                      << " bytes of type '" << paste.second << "'" << std::endl;

            if ( paste.second == "text/plain;charset=utf-8" )
            {
                std::string &t = _text.get_text();
                _cursor        = std::min( _cursor, t.size() );
                std::cout << "need to validate incoming utf8: ";
                for ( auto &x: paste.first )
                    std::cout << x;
                std::cout << std::endl;
                t.insert(
                    t.begin() + _cursor,
                    paste.first.begin(),
                    paste.first.end() );
                _cursor += paste.first.size();
            }
            else
            {
                std::string tmp( _text.get_text() );
                _cursor = std::min( _cursor, tmp.size() );

                std::insert_iterator<std::string> it(
                    tmp, tmp.begin() + long( _cursor ) );
                for ( auto &x: paste.first )
                    _cursor += utf::convert_utf8( x, it );

                _text.set_text( tmp );
            }
            invalidate();
        }
        return true;
    }
    return widget::mouse_press( e );
}

////////////////////////////////////////

} // namespace gui
