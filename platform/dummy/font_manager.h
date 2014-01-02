
#pragma once

#include <platform/font_manager.h>

namespace dummy
{

////////////////////////////////////////

class font_manager : public platform::font_manager
{
public:
	font_manager( void );
	~font_manager( void );

	std::set<std::string> get_families( void ) override;
	std::set<std::string> get_styles( void ) override;

	std::shared_ptr<draw::font> get_font( const std::string &family, const std::string &style, double pixsize ) override;
};

////////////////////////////////////////

}

