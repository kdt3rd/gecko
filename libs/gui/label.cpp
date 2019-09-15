// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "label.h"

#include "application.h"

namespace gui
{
////////////////////////////////////////

label_w::label_w( void ) {}

////////////////////////////////////////

label_w::label_w( std::string l, base::alignment a ) : _align( a )
{
    _text.set_text( std::move( l ) );
}

////////////////////////////////////////

label_w::~label_w( void ) {}

////////////////////////////////////////

void label_w::build( context &ctxt )
{
    const style &s = ctxt.get_style();
    const auto & f = s.body_font();

    // these come out in pixels...
    script::font_extents fex = f->extents();
    script::text_extents tex = f->extents( _text.get_text() );
    const coord          ht  = std::max(
        s.widget_minimum_size().h(),
        coord( fex.height ) + ctxt.from_native_vert( 2.F ) );
    const coord wt = tex.width;
    layout_target()->set_minimum( wt, ht );

    _text.set_font( f );
    if ( _bg_color.alpha() > 0.F )
        _text.set_color( s.primary_text( _bg_color ) );
    else
        _text.set_color( s.primary_text( s.background_color() ) );
}

////////////////////////////////////////

void label_w::paint( context &ctxt )
{
    if ( _bg_color.alpha() > 0.F )
    {
        _bg.set_position(
            static_cast<float>( x() ), static_cast<float>( y() ) );
        _bg.set_size(
            static_cast<float>( width() ), static_cast<float>( height() ) );
        _bg.draw( ctxt.hw_context() );
    }
    const auto &f = _text.get_font();
    if ( f )
    {
        auto pos =
            f->align_text( _text.get_text(), x1(), y1(), x2(), y2(), _align );
        _text.set_position( pos.first, pos.second );
        _text.draw( ctxt.hw_context() );
    }
}

////////////////////////////////////////

} // namespace gui
