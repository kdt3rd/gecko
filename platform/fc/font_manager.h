
#pragma once

#include <platform/font_manager.h>

namespace fc
{

////////////////////////////////////////

/// @brief FontConfig implementation of platform::font_manager
class font_manager : public platform::font_manager
{
public:
	font_manager( void );
	~font_manager( void );

	std::set<std::string> get_families( void ) override;
	std::set<std::string> get_styles( const std::string &family ) override;

	std::shared_ptr<gldraw::font> get_font( const std::string &family, const std::string &style, double pixsize ) override;

private:
	struct pimpl;
	pimpl *_impl = nullptr;
};

////////////////////////////////////////

}

