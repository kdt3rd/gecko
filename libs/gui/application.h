//
// Copyright (c) 2014 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <memory>
#include <set>
#include <map>
#include <string>
#include <functional>
#include <platform/cursor.h>
#include <platform/scancode.h>
#include "types.h"

namespace script
{
class font;
class font_manager;
}

namespace platform
{
class system;
class event;
}

namespace gui
{

class window;
class popup;
class menu;
using cursor = platform::cursor;
using standard_cursor = platform::standard_cursor;

////////////////////////////////////////

class application : public std::enable_shared_from_this<application>
{
public:
	using hotkey_handler = std::function<void(const point &)>;

	application( const std::string &display = std::string(),
				 const std::string &platform = std::string(),
				 const std::string &render = std::string() );
	virtual ~application( void );

	const std::string &active_platform( void ) { return _platform; }

	virtual bool process_quit_request( void );

	void register_global_hotkey( platform::scancode sc, hotkey_handler f );
	bool dispatch_global_hotkey( const platform::event &e );

	std::shared_ptr<window> new_window( void );

	// provided in case we track windows in the future or a sub-class
	void window_destroyed( window *w );

	std::shared_ptr<popup> new_popup( void );
	std::shared_ptr<menu> new_menu( void );

	std::shared_ptr<cursor> new_cursor( void );
	std::shared_ptr<cursor> builtin_cursor( standard_cursor sc );

	int run( void );
	void exit( int code );

	void push( void );
	void pop( void );

	std::set<std::string> get_font_families( void );
	std::set<std::string> get_font_styles( const std::string &family );
	std::shared_ptr<script::font> get_font( const std::string &family, const std::string &style, coord pixsize );

	std::shared_ptr<platform::system> get_system( void );

	static std::shared_ptr<application> current( void );

private:
	struct impl;
	std::unique_ptr<impl> _impl;

	std::string _platform;
	std::shared_ptr<script::font_manager> _fmgr;

	std::map<platform::scancode, hotkey_handler> _hotkeys;
};

////////////////////////////////////////

}

