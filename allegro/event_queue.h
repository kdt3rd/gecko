
#pragma once

#include "event.h"
#include "timer.h"
#include "keyboard.h"
#include "mouse.h"

namespace allegro
{

////////////////////////////////////////

class event_queue
{
public:
	event_queue( void )
		: m_queue( al_create_event_queue(), al_destroy_event_queue )
	{
	}

	void add_source( const display &disp )
	{
		al_register_event_source( internal(), al_get_display_event_source( disp.internal() ) );
	}

	void drop_source( const display &disp )
	{
		al_unregister_event_source( internal(), al_get_display_event_source( disp.internal() ) );
	}

	void add_source( const timer &tmr )
	{
		al_register_event_source( internal(), al_get_timer_event_source( tmr.internal() ) );
	}

	void drop_source( const timer &tmr )
	{
		al_unregister_event_source( internal(), al_get_timer_event_source( tmr.internal() ) );
	}

	void add_source( const keyboard &kb )
	{
		al_register_event_source( internal(), al_get_keyboard_event_source() );
	}

	void drop_source( const keyboard &kb )
	{
		al_unregister_event_source( internal(), al_get_keyboard_event_source() );
	}

	void add_source( const mouse &m )
	{
		al_register_event_source( internal(), al_get_mouse_event_source() );
	}

	void drop_source( const mouse &m )
	{
		al_unregister_event_source( internal(), al_get_mouse_event_source() );
	}

	bool get_next( event &ev )
	{
		return al_get_next_event( internal(), ev.internal() );
	}

	bool peek_next( event &ev )
	{
		return al_peek_next_event( internal(), ev.internal() );
	}

	bool drop_next( void )
	{
		return al_drop_next_event( internal() );
	}

	void flush( void )
	{
		al_flush_event_queue( internal() );
	}

	void wait( event &ev )
	{
		al_wait_for_event( internal(), ev.internal() );
	}

	bool wait( event &ev, float secs )
	{
		return al_wait_for_event_timed( internal(), ev.internal(), secs );
	}	

	ALLEGRO_EVENT_QUEUE *internal( void ) const { return m_queue.get(); }

private:
	shared_ptr<ALLEGRO_EVENT_QUEUE> m_queue;
};

////////////////////////////////////////

}

// vim:ft=cpp:
