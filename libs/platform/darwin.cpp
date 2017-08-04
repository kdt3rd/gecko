//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "platform.h"
#include <platform/cocoa/system.h>
//#include <platform/dummy/system.h>
#include <mutex>
#include <clocale>

namespace
{

static std::once_flag dar_init_flag;

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
	std::call_once( dar_init_flag, &initialize );

	static std::vector<platform> plat
	{
		platform( "cocoa", "gl", [](const std::string &d) { return std::make_shared<cocoa::system>(d); } ),
//	platform( "dummy", "dummy", [] { return std::make_shared<dummy::system>(); } )
	};

	return plat;
}

}
