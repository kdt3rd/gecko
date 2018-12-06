//
// Copyright (c) 2014 Ian Godin
// SPDX-License-Identifier: MIT
//

#pragma once

#include <map>
#include <draw/stretchable.h>
#include "style.h"

namespace gui
{

////////////////////////////////////////

class cocoa_style : public style
{
public:
	cocoa_style( void );
	~cocoa_style( void );

	std::shared_ptr<script::font> default_font( bool bold = false ) override;

	void background( const std::shared_ptr<draw::canvas> &c ) override;

	size button_size( const size &content ) override;
	rect button_content( const rect &size ) override;

	void button_frame( const std::shared_ptr<draw::canvas> &c, const rect &r, bool pressed ) override;

	void line_edit_frame( const std::shared_ptr<draw::canvas> &c, const rect &r, bool focused ) override;

	coord slider_size( const rect &rect ) override;
	void slider_groove( const std::shared_ptr<draw::canvas> &c, const rect &rect ) override;
	void slider_button( const std::shared_ptr<draw::canvas> &c, const rect &r, bool pressed, coord val ) override;

	virtual void text_cursor( const std::shared_ptr<draw::canvas> &c, const point &p, coord h );

private:
	void construct( const std::shared_ptr<draw::canvas> &c );
	bool _constructed = false;

	std::shared_ptr<script::font> _default_font;
	std::shared_ptr<script::font> _default_bold_font;
	std::shared_ptr<draw::stretchable> _button_frame;
	std::shared_ptr<draw::stretchable> _button_frame_down;
	std::shared_ptr<draw::stretchable> _slider_groove;
	std::shared_ptr<draw::stretchable> _slider_button;
	std::shared_ptr<draw::stretchable> _line_edit_frame;
	std::shared_ptr<draw::stretchable> _text_cursor;

//	typedef std::tuple<size,bool> button_key;
//	std::map<button_key,std::shared_ptr<draw::drawable>> _button_cache;

	std::shared_ptr<gl::texture> _grad1;
	std::shared_ptr<gl::texture> _grad2;
};

////////////////////////////////////////

}

