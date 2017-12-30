//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <platform/system.h>
#include "dispatcher.h"

namespace platform { namespace cocoa
{

////////////////////////////////////////

/// @brief Cocoa implementation of system.
class system : public ::platform::system
{
public:
	/// @brief Constructor.
	system( const std::string & );

	~system( void );

	bool is_working( void ) const override;

	opengl_query gl_proc_address( void ) override;

	std::vector<std::shared_ptr<::platform::screen>> screens( void ) override;

	std::shared_ptr<::platform::cursor> new_cursor( void ) override;
	std::shared_ptr<::platform::cursor> builtin_cursor( standard_cursor sc ) override;

	void set_selection( selection sel ) override;

	std::pair<std::vector<uint8_t>, std::string> query_selection( selection_type sel,
																  const std::vector<std::string> &allowedMimeTypes = std::vector<std::string>(),
																  const std::string &clipboardName = std::string() ) override;
	std::pair<std::vector<uint8_t>, std::string> query_selection( selection_type sel,
																  const selection_type_function &mimeSelector,
																  const std::string &clipboardName = std::string() ) override;

	const std::vector<std::string> &default_string_types( void ) override;
	selection_type_function default_string_selector( void ) override;

	mime_converter default_string_converter( void ) override;

	void begin_drag( selection sel,
					 const std::shared_ptr<::platform::cursor> &c = std::shared_ptr<::platform::cursor>() ) override;
	std::pair<std::vector<uint8_t>, std::string> query_drop( const selection_type_function &chooseMimeType = selection_type_function() ) override;

	std::shared_ptr<menu> new_system_menu( void ) override;
	std::shared_ptr<tray> new_system_tray_item( void ) override;

	std::shared_ptr<::platform::window> new_window( void ) override;
	void destroy_window( const std::shared_ptr<::platform::window> &w ) override;

	std::shared_ptr<::platform::dispatcher> get_dispatcher( void ) override;
	std::shared_ptr<::platform::keyboard> get_keyboard( void ) override;
	std::shared_ptr<::platform::mouse> get_mouse( void ) override;

	uint8_t modifier_state( void ) override;
	bool query_mouse( uint8_t &buttonMask, uint8_t &modifiers, coord_type &x, coord_type &y, int &screen ) override;

private:
	std::shared_ptr<dispatcher> _dispatcher;
	std::shared_ptr<keyboard> _keyboard;
	std::shared_ptr<mouse> _mouse;
};

////////////////////////////////////////

} }

