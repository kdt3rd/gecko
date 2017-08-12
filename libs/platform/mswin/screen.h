//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <base/size.h>
#include <platform/screen.h>
#include <string>
#include <windows.h>

namespace platform { namespace mswin
{

////////////////////////////////////////

/// @brief Microsoft Windows screen.
class screen : public platform::screen
{
public:
	screen( HMONITOR mon );
	virtual ~screen( void );

	bool is_default( void ) const override;

	double refresh_rate( void ) const override;

	base::rect bounds( bool avail ) const override;

	base::size dpi( void ) const override;

private:
	HMONITOR _monitor;
	std::string _disp_devname;
	bool _is_primary;
};

////////////////////////////////////////

} }

