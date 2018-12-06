//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT
//

#pragma once

#include <platform/system.h>
#include "dispatcher.h"
#include "platform.h"

namespace platform { namespace cocoa
{

////////////////////////////////////////

/// @brief Cocoa implementation of system.
class system : public ::platform::system
{
public:
	/// @brief Constructor.
	system( const std::string & );

	~system( void ) override;

	bool is_working( void ) const override;

	std::vector<std::shared_ptr<::platform::screen>> screens( void ) override;
	const std::shared_ptr<::platform::screen> &default_screen( void ) override;

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

	std::shared_ptr<::platform::window> new_window( window_type wintype = window_type::normal, const std::shared_ptr<::platform::screen> &s = std::shared_ptr<screen>() ) override;
	void destroy_window( const std::shared_ptr<::platform::window> &w ) override;

	std::shared_ptr<::platform::dispatcher> get_dispatcher( void ) override;

private:
	std::shared_ptr<dispatcher> _dispatcher;

	std::vector<std::shared_ptr<::platform::screen>> _screens;
};

////////////////////////////////////////

} }

