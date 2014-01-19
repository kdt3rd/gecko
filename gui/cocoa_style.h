
#pragma once

#include <map>
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

	core::size button_size( const core::size &content ) override;
	core::rect button_content( const core::rect &size ) override;

	void button_frame( const std::shared_ptr<draw::canvas> &c, const core::rect &r, bool pressed ) override;

	void line_edit_frame( const std::shared_ptr<draw::canvas> &c, const core::rect &r, bool focused ) override;

	double slider_size( const core::rect &rect ) override;
	void slider_groove( const std::shared_ptr<draw::canvas> &c, const core::rect &rect ) override;
	void slider_button( const std::shared_ptr<draw::canvas> &c, const core::rect &r, bool pressed, double val ) override;

private:
	void construct( const std::shared_ptr<draw::canvas> &c );
	bool _constructed = false;

	std::shared_ptr<draw::font> _default_font;
	std::shared_ptr<draw::font> _default_bold_font;
	std::function<std::shared_ptr<draw::drawable>( const std::shared_ptr<draw::canvas> &c, const core::rect &r )> draw_button_frame;

//	typedef std::tuple<core::size,bool> button_key;
//	std::map<button_key,std::shared_ptr<draw::drawable>> _button_cache;

	std::shared_ptr<gl::texture> _grad1;
	std::shared_ptr<gl::texture> _grad2;
};

////////////////////////////////////////

}

