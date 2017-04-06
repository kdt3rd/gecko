//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "font_manager.h"
#include "cocoa/font_manager.h"

namespace script
{

////////////////////////////////////////

void font_manager::init( void )
{
	enroll( std::make_shared<script::cocoa::font_manager>() );
}

////////////////////////////////////////

}

