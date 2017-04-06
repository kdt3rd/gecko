//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "font_manager.h"
#include <mutex>

namespace
{
	std::vector<std::shared_ptr<script::font_manager>> *managers = nullptr;
	std::mutex lock;
}

namespace script
{

////////////////////////////////////////

font_manager::font_manager( void )
{
}

////////////////////////////////////////

font_manager::~font_manager( void )
{
}

////////////////////////////////////////

namespace
{
	std::once_flag flag;
}

const std::vector<std::shared_ptr<font_manager>> &font_manager::list( void )
{
	std::call_once( flag, [](){ init(); } );

	std::lock_guard<std::mutex> guard( lock );

	if ( managers == nullptr )
		managers = new std::vector<std::shared_ptr<font_manager>>;
	return *managers;
}

////////////////////////////////////////

std::shared_ptr<font_manager> font_manager::common( void )
{
	auto mgrs = list();
	if ( !mgrs.empty() )
		return mgrs.front();
	return std::shared_ptr<font_manager>();
}

////////////////////////////////////////

void font_manager::enroll( const std::shared_ptr<font_manager> &mgr )
{
	std::lock_guard<std::mutex> guard( lock );
	if ( managers == nullptr )
		managers = new std::vector<std::shared_ptr<font_manager>>;
	managers->push_back( mgr );
}

////////////////////////////////////////

}

