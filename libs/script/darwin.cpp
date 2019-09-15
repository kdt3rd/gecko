// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "cocoa/font_manager.h"
#include "font_manager.h"

namespace script
{
////////////////////////////////////////

std::shared_ptr<font_manager> font_manager::make( void )
{
    return std::make_shared<script::cocoa::font_manager>();
}

////////////////////////////////////////

} // namespace script
