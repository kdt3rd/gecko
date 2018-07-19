//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "label.h"
#include "application.h"

namespace gui
{

////////////////////////////////////////

label_w::label_w( void )
{
}

////////////////////////////////////////

label_w::label_w( std::string l, base::alignment a )
	: _align( a )
{
	_text.set_text( std::move( l ) );
}

////////////////////////////////////////

label_w::~label_w( void )
{
}

////////////////////////////////////////

void label_w::build( context &ctxt )
{
	const style &s = ctxt.get_style();
	const auto &f = s.body_font();

	// these come out in pixels...
	script::font_extents fex = f->extents();
	script::text_extents tex = f->extents( _text.get_text() );
	const coord ht = std::max( s.widget_minimum_size().h(),
							   ctxt.from_native_vert( platform::coord_type( fex.height + 2.F ) ) );
	const coord wt = ctxt.from_native_horiz( tex.width );
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
		_bg.set_position( static_cast<float>( x() ), static_cast<float>( y() ) );
		_bg.set_size( static_cast<float>( width() ), static_cast<float>( height() ) );
		_bg.draw( ctxt.hw_context() );
	}
	const auto &f = _text.get_font();
	if ( f )
	{
//		_text.set_position( f->align_text( _text.get_text(), pix, _align ) );
		auto pix = ctxt.to_native( *this );
		using et = script::extent_type;
		auto pos = f->align_text( _text.get_text(),
								  static_cast<et>( pix.x1() ), static_cast<et>( pix.y1() ),
								  static_cast<et>( pix.x2() ), static_cast<et>( pix.y2() ),
								  _align );
		auto outp = ctxt.from_native(
			platform::point(
				static_cast<platform::coord_type>( pos.first + et(0.5) ),
				static_cast<platform::coord_type>( pos.second + et(0.5) ) ) );
		auto scalep = ctxt.from_native(
			platform::point( platform::coord_type(1), platform::coord_type(1) ) );

		_text.set_scale( scalep );
		_text.set_position( outp );
		
		_text.draw( ctxt.hw_context() );
	}
}

////////////////////////////////////////

}
