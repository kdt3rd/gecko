//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <gl/color.h>
#include <draw/colors.h>

namespace gui
{

////////////////////////////////////////

class style
{
public:
	gl::color background_color( void ) const
	{
		return _bg;
	}

	gl::color dominant_light( void ) const
	{
		return _dominant[0];
	}

	gl::color dominant_color( void ) const
	{
		return _dominant[1];
	}

	gl::color dominant_dark( void ) const
	{
		return _dominant[2];
	}

	gl::color accent_color( void ) const
	{
		return _accent;
	}

	gl::color primary_text( gl::color bg ) const;
	gl::color secondary_text( gl::color bg ) const;
	gl::color disabled_text( gl::color bg ) const;

	gl::color divider( gl::color bg ) const;

	gl::color active_icon( gl::color bg ) const;
	gl::color inactive_icon( gl::color bg ) const;

	size_t version( void ) const
	{
		return _version;
	}

	void set_background_color( const gl::color &c )
	{
		_bg = c;
		++_version;
	}

	void set_dominant_light( const gl::color &c )
	{
		_dominant[0] = c;
		++_version;
	}

	void set_dominant_normal( const gl::color &c )
	{
		_dominant[1] = c;
		++_version;
	}

	void set_dominant_dark( const gl::color &c )
	{
		_dominant[3] = c;
		++_version;
	}

	void set_accent_color( const gl::color &c )
	{
		_accent = c;
		++_version;
	}

private:
	size_t _version = 0;
	gl::color _bg = gl::white;
	gl::color _dominant[3] = { draw::indigo[2], draw::indigo[5], draw::indigo[9] };
	gl::color _accent = draw::orange[5];
};

////////////////////////////////////////

}

