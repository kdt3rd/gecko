//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <memory>
#include <map>
#include <platform/dispatcher.h>
#include "window.h"
#include "keyboard.h"
#include "mouse.h"

namespace platform { namespace mswin
{

////////////////////////////////////////

class dispatcher : public ::platform::dispatcher
{
public:
	dispatcher( const std::shared_ptr<keyboard> &k, const std::shared_ptr<mouse> &m );
	virtual ~dispatcher( void );

	int execute( void ) override;
	void exit( int code ) override;

	void add_window( const std::shared_ptr<window> &win );
	void remove_window( const std::shared_ptr<window> &win );

private:
	int _exit_code = 0;
	std::shared_ptr<keyboard> _keyboard;
	std::shared_ptr<mouse> _mouse;

	ATOM _normal_window_class = 0;
};

////////////////////////////////////////

} }

