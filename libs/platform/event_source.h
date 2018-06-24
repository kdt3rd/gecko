//
// Copyright (c) 2015 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once
#include <cstdint>
#include <string>
#include <memory>

////////////////////////////////////////

namespace platform
{

class event_target;
class event;
class system;

class event_source
{
public:
	explicit event_source( system *sys );
	virtual ~event_source( void );

	system *get_system( void ) const { return _system; }

	/// grabs the scancode / button and modifiers from the event
	/// source. if the scancode_or_button or modifiers is -1, this is
	/// interpreted as grabbing all the appropriate input from this
	/// source.
	virtual void grab( const std::shared_ptr<event_target> &et,
					   uint8_t scancode_or_button = uint8_t(-1),
					   uint8_t modifiers = uint8_t(-1) );
	virtual void ungrab( void );

	/// for devices that can display images on their panels
	virtual size_t get_image_display_count( void ) const;
	virtual size_t get_image_display_width( size_t ) const;
	virtual size_t get_image_display_height( size_t ) const;
	virtual size_t get_image_display_dpi( size_t ) const;
	virtual bool is_image_display_color( size_t ) const;
	virtual void set_image_display( size_t, void * );

	/// for input devices that can display text on their panels
	virtual size_t get_text_display_count( void ) const;
	virtual size_t get_text_num_chars_width( size_t ) const;
	virtual size_t get_text_num_chars_height( size_t ) const;
	virtual void clear_text( size_t );
	virtual void set_text( size_t, size_t, const std::string & );

	virtual size_t get_button_group_count( void ) const;

	/// @brief start will be called by the dispatcher when this
	/// event_source is finished being registered.
	///
	/// This should be thread safe as necessary
	virtual void start( void ) = 0;

	/// This will be called by the dispatcher when it is exiting or
	/// when it is being removed to allow the event_source / waitable
	/// to perform any cleanup as necessary
	///
	/// This should be thread safe as necessary
	virtual void shutdown( void ) = 0;

protected:
	std::shared_ptr<event_target> _grabber;

	/// @brief default event sender
	///
	/// if the source is grabbed, the event is immediately delivered
	/// to the grabber.
	///
	/// otherwise it is delivered to the default application
	/// dispatcher, copied onto a queue (this is thread-safe), and
	/// then delivered the next time the event loop is processed, and
	/// delivered to whatever the current (or last) event_target is
	/// active (i.e. mouse is under or was clicked into).
	void emit_event( event &e );

private:
	system *_system = nullptr;
};

} // namespace platform



