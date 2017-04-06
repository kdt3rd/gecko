//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#undef KEY_EXECUTE
#include <platform/keyboard.h>

namespace platform { namespace mswin
{

////////////////////////////////////////

class keyboard : public ::platform::keyboard
{
public:
	keyboard( void );
	virtual ~keyboard( void );
};

////////////////////////////////////////

} }

