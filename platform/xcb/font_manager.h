
#pragma once

#include <platform/font_manager.h>
#include <fontconfig/fontconfig.h>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace xcb
{

////////////////////////////////////////

/// @brief XCB implementation of platform::font_manager
class font_manager : public platform::font_manager
{
public:
	font_manager( void );
	virtual ~font_manager( void );

	virtual std::set<std::string> get_families( void );
	virtual std::set<std::string> get_styles( void );

	virtual std::shared_ptr<draw::font> get_font( const std::string &family, const std::string &style, double pixsize );

private:
	FcConfig *_config = nullptr;
	FT_Library _ft_lib;
};

////////////////////////////////////////

}

