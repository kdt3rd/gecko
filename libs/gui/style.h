//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <memory>
#include <string>
#include <script/font.h>
#include <draw/canvas.h>
#include <base/alignment.h>

namespace gui
{

////////////////////////////////////////

class style
{
public:
	style( void );
	virtual ~style( void );

	virtual std::shared_ptr<script::font> default_font( bool bold = false ) = 0;

	virtual void background( const std::shared_ptr<draw::canvas> &c ) = 0;
	virtual const base::color &label_color( void ) = 0;

	virtual base::size button_size( const base::size &content ) = 0;
	virtual base::rect button_content( const base::rect &size ) = 0;
	virtual void button_frame( const std::shared_ptr<draw::canvas> &c, const base::rect &r, bool pressed ) = 0;

	virtual void line_edit_frame( const std::shared_ptr<draw::canvas> &c, const base::rect &r, bool focused ) = 0;

	virtual double slider_size( const base::rect &r ) = 0;
	virtual void slider_groove( const std::shared_ptr<draw::canvas> &c, const base::rect &rect ) = 0;
	virtual void slider_button( const std::shared_ptr<draw::canvas> &c, const base::rect &r, bool pressed, double val ) = 0;

	virtual void text_cursor( const std::shared_ptr<draw::canvas> &c, const base::point &p, double h ) = 0;
};

////////////////////////////////////////

}
