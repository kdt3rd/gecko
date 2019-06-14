// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include "font_manager.h"
#include "fontconfig/font_manager.h"

namespace script
{
////////////////////////////////////////

std::shared_ptr<font_manager> font_manager::make( void )
{
    return std::make_shared<script::fontconfig::font_manager>();
}

////////////////////////////////////////

} // namespace script
