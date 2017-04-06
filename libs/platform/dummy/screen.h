//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <platform/screen.h>

namespace platform { namespace dummy
{

////////////////////////////////////////

class screen : public platform::screen
{
public:
	screen( void );
	~screen( void );

	base::size bounds( void ) const override;
};

////////////////////////////////////////

} }

