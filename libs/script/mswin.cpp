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

void font_manager::init( void )
{
	enroll( std::make_shared<script::mswin::font_manager>() );
}

////////////////////////////////////////

} // script



