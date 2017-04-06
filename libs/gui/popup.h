//
// Copyright (c) 2014 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "window.h"

namespace gui
{

////////////////////////////////////////

class popup : public window
{
public:
	popup( const std::shared_ptr<platform::window> &w );
	virtual ~popup( void );
};

////////////////////////////////////////

}

