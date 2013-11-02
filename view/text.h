
#pragma once

#include "view.h"

namespace view
{

////////////////////////////////////////

class text : public view
{
public:
	text( const std::shared_ptr<draw::area> &a );
	virtual ~text( void );

	void set_font( const std::shared_ptr<draw::font> &fnt ) { _font = fnt; }
	void set_color( const draw::color &c ) { _color = c; }
	void set_text( std::string txt ) { _text = std::move( txt ); }

	virtual void paint( const std::shared_ptr<draw::canvas> &canvas );

private:
	std::shared_ptr<draw::area> _area;
	std::shared_ptr<draw::font> _font;
	draw::color _color;
	std::string _text;
};

////////////////////////////////////////

}

