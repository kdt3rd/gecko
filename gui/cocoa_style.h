
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

private:
	std::shared_ptr<draw::font> _default_font;
};

////////////////////////////////////////

}

