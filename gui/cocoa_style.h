
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

	virtual std::shared_ptr<draw::font> default_font( bool bold = false );

	virtual void background( const std::shared_ptr<draw::canvas> &c, const draw::rect &r );

	virtual void button_frame( const std::shared_ptr<draw::canvas> &c, const draw::rect &r, bool pressed );

	virtual double slider_size( const draw::rect &rect );
	virtual void slider_groove( const std::shared_ptr<draw::canvas> &c, const draw::rect &rect );
	virtual void slider_button( const std::shared_ptr<draw::canvas> &c, const draw::rect &r, bool pressed, double val );

private:
	std::shared_ptr<draw::font> _default_font;
	std::shared_ptr<draw::font> _default_bold_font;
};

////////////////////////////////////////

}

