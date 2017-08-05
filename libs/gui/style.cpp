//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "style.h"

namespace gui
{

////////////////////////////////////////

void style::set_font_manager( const std::shared_ptr<script::font_manager> &fmgr )
{
	_fmgr = fmgr;
	_display = _fmgr->get_font( "Arial", "Bold", 24.0 );
	_title = _fmgr->get_font( "Arial", "Bold", 16.0 );
	_body = _fmgr->get_font( "Arial", "Bold", 12.0 );
	_caption = _fmgr->get_font( "Arial", "Bold", 10.0 );
}

////////////////////////////////////////

gl::color style::primary_text( gl::color bg ) const
{
	float w = bg.distance( gl::white );
	float b = bg.distance( gl::black );
	if ( w > b )
		return gl::color( 1, 1, 1, 1.00 );
	else
		return gl::color( 0, 0, 0, 0.87 );
}

////////////////////////////////////////

gl::color style::secondary_text( gl::color bg ) const
{
	float w = bg.distance( gl::white );
	float b = bg.distance( gl::black );
	if ( w > b )
		return gl::color( 1, 1, 1, 0.70 );
	else
		return gl::color( 0, 0, 0, 0.54 );
}

////////////////////////////////////////

gl::color style::disabled_text( gl::color bg ) const
{
	float w = bg.distance( gl::white );
	float b = bg.distance( gl::black );
	if ( w > b )
		return gl::color( 1, 1, 1, 0.50 );
	else
		return gl::color( 0, 0, 0, 0.38 );
}

////////////////////////////////////////

gl::color style::divider( gl::color bg ) const
{
	float w = bg.distance( gl::white );
	float b = bg.distance( gl::black );
	if ( w > b )
		return gl::color( 1, 1, 1, 0.12 );
	else
		return gl::color( 0, 0, 0, 0.12 );
}

////////////////////////////////////////

gl::color style::active_icon( gl::color bg ) const
{
	float w = bg.distance( gl::white );
	float b = bg.distance( gl::black );
	if ( w > b )
		return gl::color( 1, 1, 1, 1.00 );
	else
		return gl::color( 0, 0, 0, 0.54 );
}

////////////////////////////////////////

gl::color style::inactive_icon( gl::color bg ) const
{
	float w = bg.distance( gl::white );
	float b = bg.distance( gl::black );
	if ( w > b )
		return gl::color( 1, 1, 1, 0.50 );
	else
		return gl::color( 0, 0, 0, 0.38 );
}

////////////////////////////////////////

}
