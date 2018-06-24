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

	std::vector<std::shared_ptr<::platform::screen>> screens( void ) override { return _screens; }
	const std::shared_ptr<::platform::screen> &default_screen( void ) override;

	std::shared_ptr<cursor> new_cursor( void ) override;

	std::shared_ptr<cursor> builtin_cursor( standard_cursor sc ) override;

	void set_selection( selection sel ) override;

	/// @brief query system selection
	///
	/// when the selection type @param sel is custom, this indicates use of a
	/// custom clipboard name, at which point the clipboardName must
	/// be provided. otherwise, it may be left at default.
	///
	/// if the @param allowedMimeTypes list is empty, a utf-8 string
	/// will be requested.
	///
	/// @return the selection present in the indicated selection type
	/// or clipboard, along with the selected mime type.
	///
	std::pair<std::vector<uint8_t>, std::string> query_selection( selection_type sel,
																  const std::vector<std::string> &allowedMimeTypes = std::vector<std::string>(),
																  const std::string &clipboardName = std::string() ) override;

	/// @brief query system selection
	///
	/// Similar to @sa query_selection, but instead of providing a
	/// list of allowed mime types, allows dynamic selection of mime
	/// type based on what is in the selection.
	///
	/// @return the selection present in the indicated selection type
	/// or clipboard, along with the selected mime type.
	///
	std::pair<std::vector<uint8_t>, std::string> query_selection( selection_type sel,
																  const selection_type_function &mimeSelector,
																  const std::string &clipboardName = std::string() ) override;

	/// @brief provides a list of mime types and synonyms common to the platform
	const std::vector<std::string> &default_string_types( void ) override;
	/// @brief provides a default function to select a utf8 string
	selection_type_function default_string_selector( void ) override;

	mime_converter default_string_converter( void ) override;

	/// @endgroup

	/// @group Drag and Drop interface

	/// @brief begin a drag motion.
	///
	/// Event processing continues as normal, but this routine will
	/// not return until the user releases the mouse button.
	void begin_drag( selection sel,
					 const std::shared_ptr<cursor> &cursor = std::shared_ptr<cursor>() ) override;

	/// @brief query available mime types in response to a drop request event
	std::pair<std::vector<uint8_t>, std::string> query_drop( const selection_type_function &chooseMimeType = selection_type_function() ) override;

	std::shared_ptr<::platform::menu> new_system_menu( void ) override;
	std::shared_ptr<::platform::tray> new_system_tray_item( void ) override;

	std::shared_ptr<::platform::window> new_window( const std::shared_ptr<::platform::screen> &s = std::shared_ptr<::platform::screen>() ) override;
	void destroy_window( const std::shared_ptr<::platform::window> &w ) override;

	std::shared_ptr<::platform::dispatcher> get_dispatcher( void ) override;

private:
	std::shared_ptr<dispatcher> _dispatcher;
	std::vector<std::shared_ptr<::platform::screen>> _screens;
};

////////////////////////////////////////

} }

