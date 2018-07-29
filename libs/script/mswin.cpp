//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

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



