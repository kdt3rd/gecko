
#pragma once

#include <platform/font_manager.h>
#include <fontconfig/fontconfig.h>

namespace xcb
{

////////////////////////////////////////

class font_manager : public platform::font_manager
{
public:
	font_manager( void );
	virtual ~font_manager( void );

	virtual std::set<std::string> get_families( void );
	virtual std::set<std::string> get_styles( void );

private:
	FcConfig *_config = nullptr;
};

////////////////////////////////////////

}

