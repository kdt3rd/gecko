
#include "font_manager.h"
#include <draw/dummy/font.h>

#include <iostream>
#include <stdexcept>
#include <sstream>

namespace dummy
{

////////////////////////////////////////

font_manager::font_manager( void )
{
	set_manager_name( "dummy" );
	set_manager_version( "1" );
}

////////////////////////////////////////

font_manager::~font_manager( void )
{
}

////////////////////////////////////////

std::set<std::string> font_manager::get_families( void )
{
	std::set<std::string> ret;
	return ret;
}

////////////////////////////////////////

std::set<std::string> font_manager::get_styles( const std::string &family )
{
	std::set<std::string> ret;
	return ret;
}

////////////////////////////////////////

std::shared_ptr<draw::font> font_manager::get_font( const std::string &family, const std::string &style, double pixsize )
{
	auto ret = std::make_shared<dummy::font>( family, style, pixsize );
	return ret;
}

////////////////////////////////////////

}

