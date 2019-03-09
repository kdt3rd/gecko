// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include <memory>

////////////////////////////////////////

namespace platform
{

class event;
class event_source;

///
/// @brief Class event_target provides...
///
class event_target
{
public:
	event_target( void );
	event_target( const event_target & ) = delete;
	event_target( event_target && ) = delete;
	event_target &operator=( const event_target & ) = delete;
	event_target &operator=( event_target && ) = delete;

	virtual ~event_target( void );

	virtual bool process_event( const event & ) = 0;

private:
};

} // namespace platform



