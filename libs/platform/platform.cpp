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

platform::platform( std::string name, std::string render, const std::function<std::shared_ptr<system>(void)> &creator )
	: _name( std::move(name) ), _render( std::move(render) ), _creator( creator )
{
}

////////////////////////////////////////

const std::vector<platform> &platform::list( void )
{
	return init();
}

////////////////////////////////////////

const platform &platform::common( void )
{
	return init().front();
}

////////////////////////////////////////

}

