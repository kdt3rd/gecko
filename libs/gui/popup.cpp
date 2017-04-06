//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "popup.h"
#include <platform/window.h>

namespace gui
{

////////////////////////////////////////

popup::popup( const std::shared_ptr<platform::window> &w )
	: window( w )
{
	precondition( bool(_window), "null window" );
	_window->set_popup();
}


////////////////////////////////////////


popup::~popup( void )
{
}


////////////////////////////////////////


}

