//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "platform.h"
#include <platform/xlib/system.h>
//#include <platform/wayland/system.h>
//#include <platform/xcb/system.h>
//#include <platform/dummy/system.h>
#include <clocale>
#include <mutex>

////////////////////////////////////////

namespace
{

static std::once_flag lin_init_flag;

static void initialize( void )
{
	std::setlocale( LC_ALL, "" );
}

}

namespace platform
{

////////////////////////////////////////

std::vector<platform> &platform::init( void )
{
	std::call_once( lin_init_flag, &initialize );

	static std::vector<platform> plat
	{
		platform( "xlib", "gl", [] { return std::make_shared<xlib::system>(); } ),
//		platform( "wayland", "gl", [] { return std::make_shared<wayland::system>(); } )
	};

	return plat;
}

}
