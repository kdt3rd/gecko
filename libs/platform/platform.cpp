// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#include "platform.h"

////////////////////////////////////////

namespace platform
{

////////////////////////////////////////

platform::platform( std::string name, std::string render, const std::function<std::shared_ptr<system>(const std::string &)> &creator )
	: _name( std::move(name) ), _render( std::move(render) ), _creator( creator )
{
}

////////////////////////////////////////

const std::vector<platform> &platform::list( void )
{
	return init();
}

////////////////////////////////////////

const platform &
platform::preferred( void )
{
	return init().front();
}

////////////////////////////////////////

std::shared_ptr<system>
platform::find_running( const std::string &disp )
{
	for ( auto &p: init() )
	{
		auto result = p.create( disp );
		if ( result->is_working() )
			return result;
	}

	throw std::runtime_error( "no display system found" );
}

////////////////////////////////////////

}

