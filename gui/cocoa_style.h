
#pragma once

#include "style.h"

namespace gui
{

////////////////////////////////////////

class cocoa_style : public style
{
public:
	cocoa_style( void );
	virtual ~cocoa_style( void );

	virtual std::shared_ptr<draw::font> default_font( void );

	virtual std::shared_ptr<view::view> bg_frame( const std::shared_ptr<layout::area> &area );

	virtual std::shared_ptr<view::view> button_frame( const std::shared_ptr<layout::area> &area );
	virtual std::shared_ptr<view::view> button_text( const std::shared_ptr<layout::area> &area, const std::string &txt );

	virtual std::shared_ptr<view::view> label_text( const std::shared_ptr<layout::area> &area, const std::string &txt );

private:
	std::shared_ptr<draw::font> _default_font;
};

////////////////////////////////////////

}

