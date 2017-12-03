//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <platform/cursor.h>
#include <X11/Xlib.h>

////////////////////////////////////////

namespace platform { namespace xlib
{

///
/// @brief Class cursor provides...
///
class cursor : public ::platform::cursor
{
public:
	// TODO: once we support creating cursors, pass in display
	// for now just act as holder
	explicit cursor( Cursor c );

	cursor( void ) = delete;
	cursor( const cursor &c ) = delete;
	cursor( cursor &&c ) = delete;
	cursor &operator=( const cursor &c ) = delete;
	cursor &operator=( cursor &&c ) = delete;

	virtual ~cursor( void );

	bool supports_color( void ) const override;
	bool supports_animation( void ) const override;

	bool is_animated( void ) const override;

	Cursor handle( void ) const { return _handle; }
private:

	Cursor _handle;
};

} // namespace xlib
} // namespace platform


