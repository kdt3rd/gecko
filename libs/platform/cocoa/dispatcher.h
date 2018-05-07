//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <memory>
#include <map>
#include <vector>
#include <platform/dispatcher.h>
#include "window.h"
#include "keyboard.h"
#include "mouse.h"

namespace platform { namespace cocoa
{

////////////////////////////////////////

/// @brief Cocoa implementation of dispatcher.
class dispatcher : public ::platform::dispatcher
{
public:
	dispatcher( const std::shared_ptr<keyboard> &k, const std::shared_ptr<mouse> &m );
	~dispatcher( void ) override;

	int execute( void ) override;
	void exit( int code ) override;

	void add_waitable( const std::shared_ptr<waitable> &w ) override;
	void remove_waitable( const std::shared_ptr<waitable> &w ) override;

	void add_window( const std::shared_ptr<window> &w );
	void remove_window( const std::shared_ptr<window> &w );

private:
	int _exit_code = 0;
//	bool _continue_running = true;
	std::shared_ptr<keyboard> _keyboard;
	std::shared_ptr<mouse> _mouse;
	std::map<void*,std::shared_ptr<window>> _windows;
};

////////////////////////////////////////

} }

