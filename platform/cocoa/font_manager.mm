
#include "font_manager.h"

#include <iostream>
#include <stdexcept>
#include <sstream>

#include <draw/cairo/font.h>

namespace cocoa
{

////////////////////////////////////////

font_manager::font_manager( void )
{
}

////////////////////////////////////////

font_manager::~font_manager( void )
{
}

////////////////////////////////////////

std::set<std::string> font_manager::get_foundries( void )
{
	std::set<std::string> ret;
	return ret;
}

////////////////////////////////////////

std::set<std::string> font_manager::get_families( void )
{
	std::set<std::string> ret;
	return ret;
}

////////////////////////////////////////

std::set<std::string> font_manager::get_styles( void )
{
	std::set<std::string> ret;
	return ret;
}

////////////////////////////////////////

std::shared_ptr<draw::font> font_manager::get_font( const std::string &family, const std::string &style, double pixsize )
{
	std::shared_ptr<cairo::font> ret;
	return ret;
}

////////////////////////////////////////

}

