//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "font_manager.h"

#include <iostream>
#include <stdexcept>
#include <sstream>

namespace platform { namespace dummy
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

std::shared_ptr<gldraw::font> font_manager::get_font( const std::string &family, const std::string &style, double pixsize )
{
	return std::shared_ptr<gldraw::font>();
}

////////////////////////////////////////

} }

