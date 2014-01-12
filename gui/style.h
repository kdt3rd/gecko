
#pragma once

#include <memory>
#include <string>
#include <gldraw/font.h>
#include <gldraw/canvas.h>
#include <core/alignment.h>

namespace gui
{

////////////////////////////////////////

class style
{
public:
	style( void );
	virtual ~style( void );

	virtual std::shared_ptr<gldraw::font> default_font( bool bold = false ) = 0;

	virtual void background( const std::shared_ptr<gldraw::canvas> &c ) = 0;

	virtual core::size button_size( const core::size &content ) = 0;
	virtual core::rect button_content( const core::rect &size ) = 0;
	virtual void button_frame( const std::shared_ptr<gldraw::canvas> &c, const core::rect &r, bool pressed ) = 0;

	virtual void line_edit_frame( const std::shared_ptr<gldraw::canvas> &c, const core::rect &r, bool focused ) = 0;

	virtual double slider_size( const core::rect &r ) = 0;
	virtual void slider_groove( const std::shared_ptr<gldraw::canvas> &c, const core::rect &rect ) = 0;
	virtual void slider_button( const std::shared_ptr<gldraw::canvas> &c, const core::rect &r, bool pressed, double val ) = 0;
};

////////////////////////////////////////

}
