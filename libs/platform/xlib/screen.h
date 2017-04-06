//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <platform/screen.h>
#include <X11/Xlib.h>
#include <memory>

namespace platform { namespace xlib
{

////////////////////////////////////////

/// @brief X11 implementation of platform::screen
class screen : public platform::screen
{
public:
	/// @brief Constructor.
	screen( const std::shared_ptr<Display> &disp, int scr );
	~screen( void );

	bool is_default( void ) const override;

	base::size bounds( void ) const override;

	base::size dpi( void ) const override;

private:
	std::shared_ptr<Display> _display;
	int _screen = 0;
};

////////////////////////////////////////

} }

