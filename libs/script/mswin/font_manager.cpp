//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "font_manager.h"
#include "font.h"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <tuple>
#include <map>
#include <vector>
#include <base/scope_guard.h>
#include <base/language.h>
#include <base/contract.h>

namespace script { namespace mswin
{

struct font_manager::pimpl
{
	char dummy[2];
};

////////////////////////////////////////

font_manager::font_manager( void )
	: _impl( new pimpl )
{
	throw_not_yet();
}

////////////////////////////////////////

font_manager::~font_manager( void )
{
}

////////////////////////////////////////

std::set<std::string> font_manager::get_families( void )
{
	std::set<std::string> ret;

	throw_not_yet();
	return ret;
}

////////////////////////////////////////

std::set<std::string> font_manager::get_styles( const std::string &family )
{
	std::set<std::string> ret;

	throw_not_yet();
	return ret;
}

////////////////////////////////////////

std::shared_ptr<script::font>
font_manager::get_font( const std::string &family, const std::string &style, double pixsize )
{
	std::string lang = base::locale::language();

	std::shared_ptr<script::font> ret;

	throw_not_yet();
	return ret;
}

////////////////////////////////////////

} }

