
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
		: m_queue( callegro::al_create_event_queue(), callegro::al_destroy_event_queue )
	{
	}

	void add_source( const display &disp )
	{
		callegro::al_register_event_source( internal(), callegro::al_get_display_event_source( disp.internal() ) );
	}

	void drop_source( const display &disp )
	{
		callegro::al_unregister_event_source( internal(), callegro::al_get_display_event_source( disp.internal() ) );
	}

	void add_source( const timer &tmr )
	{
		callegro::al_register_event_source( internal(), callegro::al_get_timer_event_source( tmr.internal() ) );
	}

	void drop_source( const timer &tmr )
	{
		callegro::al_unregister_event_source( internal(), callegro::al_get_timer_event_source( tmr.internal() ) );
	}

	void add_source( const keyboard &kb )
	{
		callegro::al_register_event_source( internal(), callegro::al_get_keyboard_event_source() );
	}

	void drop_source( const keyboard &kb )
	{
		callegro::al_unregister_event_source( internal(), callegro::al_get_keyboard_event_source() );
	}

	void add_source( const mouse &m )
	{
		callegro::al_register_event_source( internal(), callegro::al_get_mouse_event_source() );
	}

	void drop_source( const mouse &m )
	{
		callegro::al_unregister_event_source( internal(), callegro::al_get_mouse_event_source() );
	}

	bool get_next( event &ev )
	{
		return callegro::al_get_next_event( internal(), ev.internal() );
	}

	bool peek_next( event &ev )
	{
		return callegro::al_peek_next_event( internal(), ev.internal() );
	}

	bool drop_next( void )
	{
		return callegro::al_drop_next_event( internal() );
	}

	void flush( void )
	{
		callegro::al_flush_event_queue( internal() );
	}

	void wait( event &ev )
	{
		callegro::al_wait_for_event( internal(), ev.internal() );
	}

	bool wait( event &ev, float secs )
	{
		return callegro::al_wait_for_event_timed( internal(), ev.internal(), secs );
	}	

	callegro::ALLEGRO_EVENT_QUEUE *internal( void ) const { return m_queue.get(); }

private:
	std::shared_ptr<callegro::ALLEGRO_EVENT_QUEUE> m_queue;
};

////////////////////////////////////////

}

// vim:ft=cpp:
