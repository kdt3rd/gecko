// Copyright (c) 2014 Ian Godin
// SPDX-License-Identifier: MIT

#include "dark_style.h"

#include "application.h"

#include <base/contract.h>
#include <draw/composite.h>
#include <draw/object.h>
#include <draw/polylines.h>
#include <draw/shaders.h>
#include <draw/stretchable.h>

namespace
{
gui::color bg{ 0.13F, 0.13F, 0.13F };
gui::color fg{ 1.0F, 1.0F, 1.0F };
gui::color button_bg{ 0.27, 0.27, 0.27 };

} // namespace

////////////////////////////////////////

namespace gui
{
////////////////////////////////////////

dark_style::dark_style( void )
{
    _default_font =
        application::current()->get_font( "Lucida Grande", "Regular", 7.0 );
    postcondition( bool( _default_font ), "font for dark style not found" );

    _default_bold_font =
        application::current()->get_font( "Lucida Grande", "Bold", 7.0 );
    postcondition(
        bool( _default_bold_font ), "font for dark bold style not found" );
}

////////////////////////////////////////

dark_style::~dark_style( void ) {}

////////////////////////////////////////

std::shared_ptr<script::font> dark_style::default_font( bool bold )
{
    if ( bold )
        return _default_bold_font;
    else
        return _default_font;
}

////////////////////////////////////////

void dark_style::background( const std::shared_ptr<draw::canvas> &c )
{
    c->clear_color( bg );
    c->clear();
}

////////////////////////////////////////

const color &dark_style::label_color( void ) { return fg; }

////////////////////////////////////////

size dark_style::button_size( const size &content )
{
    size full( content );
    full.grow( 12, 6 );
    full.ceil();
    full.set_height( std::max( full.h(), 21.0 ) );
    return full;
}

////////////////////////////////////////

rect dark_style::button_content( const rect &full )
{
    rect content( full );
    content.shrink( 6, 6, 3, 3 );
    return content;
}

////////////////////////////////////////

void dark_style::button_frame(
    const std::shared_ptr<draw::canvas> &c, const rect &r, bool pressed )
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

void dark_style::line_edit_frame(
    const std::shared_ptr<draw::canvas> &c, const rect &r, bool focused )
{
    construct( c );
    _line_edit_frame->set( c, r );
    _line_edit_frame->draw( *c );
}

////////////////////////////////////////

coord dark_style::slider_size( const rect &rect ) { return rect.radius(); }

////////////////////////////////////////

void dark_style::slider_groove(
    const std::shared_ptr<draw::canvas> &c, const rect &rect )
{
    construct( c );

    coord rad = slider_size( rect );
    coord h   = rect.height() - 7;
    rect  tmp( rect );
    tmp.trim( rad, rad, h / 2, h / 2 );

    _slider_groove->set( c, tmp );
    _slider_groove->draw( *c );
}

////////////////////////////////////////

void dark_style::slider_button(
    const std::shared_ptr<draw::canvas> &c,
    const rect &                         r,
    bool                                 pressed,
    coord                                val )
{
    construct( c );

    coord rad = 9.0; //r.radius();
    rect  tmp( rad * 2, rad * 2 );
    tmp.set_center( { r.x( val, rad ), r.y( 0.5, rad ) } );

    _slider_button->set( c, tmp );
    _slider_button->draw( *c );
}

////////////////////////////////////////

void dark_style::text_cursor(
    const std::shared_ptr<draw::canvas> &c, const point &p, coord h )
{
    construct( c );

    rect tmp( p - point( 0, h ), 2, h );

    _text_cursor->set( c, tmp );
    _text_cursor->draw( *c );
}

////////////////////////////////////////

void dark_style::construct( const std::shared_ptr<draw::canvas> &c )
{
    if ( !_constructed )
    {
        // Button drawing
        _button_frame      = std::make_shared<draw::stretchable>();
        _button_frame_down = std::make_shared<draw::stretchable>();
        {
            base::path path;
            path.rounded_rect( { 0, 0 }, 20, 20, 3 );

            base::paint paint;
            paint.set_fill_color( button_bg );

            _button_frame->create( c, path, paint, { 10, 10 } );
            _button_frame_down->create( c, path, paint, { 10, 10 } );
        }

        // Slider groove
        _slider_groove = std::make_shared<draw::stretchable>();
        {
            base::path path;
            path.rounded_rect( { 0, 0 }, { 20, 7 }, 2 );

            base::paint paint;
            paint.set_fill_color( button_bg );

            _slider_groove->create( c, path, paint, { 10, 3.5 } );
        }

        // Slider button drawing
        _slider_button = std::make_shared<draw::stretchable>();
        {
            base::path path;
            path.circle( { 10, 10 }, 9 );

            base::paint paint;
            paint.set_fill_color( button_bg );
            //			paint.set_fill_linear( { 0, 0 }, { 0, 20 }, grad1 );

            _slider_button->create( c, path, paint, { 10, 10 } );
        }

        // Line edit frame
        _line_edit_frame = std::make_shared<draw::stretchable>();
        {
            base::paint paint( button_bg, 1.0 );
            paint.set_fill_color( { 1, 1, 1 } );

            base::path path;
            path.rectangle( { 0, 0 }, 20, 20 );

            _line_edit_frame->create( c, path, paint, { 10, 10 } );
        }

        // Text cursor
        _text_cursor = std::make_shared<draw::stretchable>();
        {
            base::path path;
            path.move_to( { 0.5, 0 } );
            path.line_by( { 0, 10 } );

            base::paint pen( { 0, 0, 0, 1 }, 1.0 );

            _text_cursor->create( c, path, pen, { 0.5, 5 } );
        }

        _constructed = true;
    }
}

////////////////////////////////////////

} // namespace gui
