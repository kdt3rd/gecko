
#pragma once

#include <memory>
#include <string>
#include <draw/font.h>
#include <draw/canvas.h>
#include <core/alignment.h>

namespace gui
{

////////////////////////////////////////

class style
{
public:
	style( void );
	virtual ~style( void );

	virtual std::shared_ptr<draw::font> default_font( bool bold = false ) = 0;

	virtual void button_frame( const std::shared_ptr<draw::canvas> &c, const draw::rect &r, bool pressed ) = 0;

	virtual void slider_groove( const std::shared_ptr<draw::canvas> &c, const draw::rect &rect ) = 0;
	virtual void slider_button( const std::shared_ptr<draw::canvas> &c, const draw::rect &r, bool pressed ) = 0;
};

////////////////////////////////////////

}
