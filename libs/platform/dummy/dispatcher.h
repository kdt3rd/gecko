//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT
//

#pragma once

#include <memory>
#include <map>
#include <platform/dispatcher.h>
#include "window.h"
#include "keyboard.h"
#include "mouse.h"

namespace platform { namespace dummy
{

////////////////////////////////////////

class dispatcher : public platform::dispatcher
{
public:
	dispatcher( const std::shared_ptr<keyboard> &k, const std::shared_ptr<mouse> &m );
	~dispatcher( void );

	int execute( void ) override;
	void exit( int code ) override;

private:
	int _exit_code = 0;
	std::shared_ptr<keyboard> _keyboard;
	std::shared_ptr<mouse> _mouse;
};

////////////////////////////////////////

} }
