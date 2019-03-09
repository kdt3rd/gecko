// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "font_manager.h"
#include "mswin/font_manager.h"

////////////////////////////////////////

namespace script
{

std::shared_ptr<font_manager> font_manager::make( void )
{
	return std::make_shared<script::mswin::font_manager>();
}

////////////////////////////////////////

} // script



