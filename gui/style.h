
#pragma once

#include <memory>
#include <string>
#include <draw/font.h>

namespace gui
{

////////////////////////////////////////

class style
{
public:
	style( void );
	virtual ~style( void );

	virtual std::shared_ptr<draw::font> default_font( void ) = 0;
};

////////////////////////////////////////

}
