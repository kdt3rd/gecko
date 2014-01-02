
#pragma once

#include "style.h"

namespace gui
{

////////////////////////////////////////

class cocoa_style : public style
{
public:
	cocoa_style( void );
	~cocoa_style( void );

	std::shared_ptr<draw::font> default_font( bool bold = false ) override;

	void background( const std::shared_ptr<draw::canvas> &c ) override;

	draw::size button_size( const draw::size &content ) override;
	draw::rect button_content( const draw::rect &size ) override;
	void button_frame( const std::shared_ptr<draw::canvas> &c, const draw::rect &r, bool pressed ) override;

	void line_edit_frame( const std::shared_ptr<draw::canvas> &c, const draw::rect &r, bool focused ) override;

	double slider_size( const draw::rect &rect ) override;
	void slider_groove( const std::shared_ptr<draw::canvas> &c, const draw::rect &rect ) override;
	void slider_button( const std::shared_ptr<draw::canvas> &c, const draw::rect &r, bool pressed, double val ) override;

private:
	std::shared_ptr<draw::font> _default_font;
	std::shared_ptr<draw::font> _default_bold_font;
};

////////////////////////////////////////

}

