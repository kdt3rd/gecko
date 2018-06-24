//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "event_source.h"
#include "event_target.h"

////////////////////////////////////////

namespace platform
{

////////////////////////////////////////

event_source::event_source( system *s )
	: _system( s )
{
}

////////////////////////////////////////

event_source::~event_source( void )
{
}

////////////////////////////////////////

void
event_source::grab( const std::shared_ptr<event_target> &et, uint8_t scancode_or_button, uint8_t modifiers )
{
	_grabber = et;
}

////////////////////////////////////////

void
event_source::ungrab( void )
{
	_grabber.reset();
}

////////////////////////////////////////

size_t
event_source::get_image_display_count( void ) const
{
	return 0;
}

////////////////////////////////////////

size_t
event_source::get_image_display_width( size_t ) const
{
	return 0;
}

////////////////////////////////////////

size_t
event_source::get_image_display_height( size_t ) const
{
	return 0;
}

////////////////////////////////////////

size_t
event_source::get_image_display_dpi( size_t ) const
{
	return 0;
}

////////////////////////////////////////

bool
event_source::is_image_display_color( size_t ) const
{
	return false;
}

////////////////////////////////////////

void
event_source::set_image_display( size_t, void * )
{
}

////////////////////////////////////////

size_t
event_source::get_text_display_count( void ) const
{
	return 0;
}

////////////////////////////////////////

size_t
event_source::get_text_num_chars_width( size_t ) const
{
	return 0;
}

////////////////////////////////////////

size_t
event_source::get_text_num_chars_height( size_t ) const
{
	return 0;
}

////////////////////////////////////////

void event_source::clear_text( size_t )
{
}

////////////////////////////////////////

void
event_source::set_text( size_t, size_t, const std::string & )
{
}

////////////////////////////////////////

size_t event_source::get_button_group_count( void ) const
{
	return 0;
}

} // platform



