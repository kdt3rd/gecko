//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

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
		try
		{
			return p.create( disp );
		}
		catch ( std::exception &e )
		{
			std::cerr << "Unable to connect to '" << p.name() << "' (renderer '" << p.renderer() << "'): " << e.what() << std::endl;
		}
	}

	throw std::runtime_error( "Unable to find any enabled or running display system for the current platform" );
}

////////////////////////////////////////

}

