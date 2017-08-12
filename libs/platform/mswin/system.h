//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <platform/system.h>
#include "dispatcher.h"

namespace platform { namespace mswin
{

////////////////////////////////////////

class system : public ::platform::system
{
public:
	system( const std::string & );
	virtual ~system( void );

	bool is_working( void ) const override { return true; }

	opengl_query gl_proc_address( void ) override;

	std::vector<std::shared_ptr<::platform::screen>> screens( void ) override { return _screens; }
	std::shared_ptr<::platform::window> new_window( void ) override;
	void destroy_window( const std::shared_ptr<::platform::window> &w ) override;

	std::shared_ptr<::platform::dispatcher> get_dispatcher( void ) override;
	std::shared_ptr<::platform::keyboard> get_keyboard( void ) override;
	std::shared_ptr<::platform::mouse> get_mouse( void ) override;

private:
	std::shared_ptr<dispatcher> _dispatcher;
	std::shared_ptr<keyboard> _keyboard;
	std::shared_ptr<mouse> _mouse;
	std::vector<std::shared_ptr<::platform::screen>> _screens;
};

////////////////////////////////////////

} }

