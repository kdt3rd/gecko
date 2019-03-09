// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <vector>
#include <functional>

////////////////////////////////////////

namespace platform
{

enum class selection_type
{
	MOUSE,
	CLIPBOARD,
	CUSTOM
};

/// Typedef for a function that, given a list of mime types, returns the mime type desired
using selection_type_function = std::function<std::string (const std::vector<std::string> &)>;

///
/// @brief Class selection provides a mechanism to store and convert a
/// user selection such that it can be provided to other applications.
///
class selection
{
public:
	/// function to convert to mime types.
	using data_container = std::vector<uint8_t>;
	using mime_converter = std::function<data_container (const data_container &, const std::string &, const std::string &)>;

	selection( void ) = default;
	selection( const selection & ) = default;
	selection( selection && ) = default;
	selection &operator=( const selection & ) = default;
	selection &operator=( selection && ) = default;

	/// Used to indicate an empty / none selection, which should have
	/// the effect of clearing any active selection in that slot
	selection( selection_type st, const std::string &customSelType = std::string() );

	/// simple constructor to provide a selection to the rest of the O.S. as a UTF8 string
	///
	/// When the selection type is mouse or clipboard, no name need be
	/// provided, only if the application wishes to use a custom
	/// clipboard
	selection( selection_type st, const std::string &s, const std::string &customSelType = std::string() );

	/// full specification of a selection.
	///
	/// A more expanded version of a selection, this allows for
	/// rich selection transfer between applications it is expected
	/// that the mime converter function will remain valid until one of a few
	/// conditions happens:
	/// - another selection is set
	/// - another program gains the selection
	/// - the program exits
	selection( selection_type st,
			   const data_container &data, const std::string &mime_type,
			   const std::vector<std::string> &avail_mime,
			   const mime_converter &converter,
			   const std::string &customSelType = std::string() );
	~selection( void );

	selection_type which_clip( void ) const { return _type; }
	const std::string &custom_clipboard( void ) const { return _custom_selection_name; }

	const std::vector<std::string> &available_mime_types( void ) const { return _avail_mime; }

	const std::string &current_mime_type( void ) const;

	/// some knowledge of the built-in conversions and synonyms for types is used to
	/// provide default conversions to avoid burdening the converter function from
	/// having to decorate or otherwise handle the common types
	const std::vector<uint8_t> &as( const std::string &mime_type );

	void clear( void );

private:
	data_container _data;
	std::string _data_type;

	data_container _convert_cache;
	std::string _cache_type;

	std::vector<std::string> _avail_mime;
	mime_converter _convert_func;

	selection_type _type = selection_type::MOUSE;
	std::string _custom_selection_name;
};

} // namespace platform



