
#pragma once

extern "C" {
#include <allegro5/allegro.h>
}

namespace allegro
{

////////////////////////////////////////

class timer
{
public:
	timer( double secs )
		: m_timer( al_create_timer( secs ), al_destroy_timer )
	{
	}

	double timer_speed( void ) const
	{
		return al_get_timer_speed( internal() );
	}

	void set_timer_speed( double secs ) const
	{
		return al_set_timer_speed( internal(), secs );
	}

	void start( void )
	{
		al_start_timer( internal() );
	}

	void stop( void )
	{
		al_stop_timer( internal() );
	}

	bool started( void ) const
	{
		return al_get_timer_started( internal() );
	}

	int64_t count( void ) const
	{
		return al_get_timer_count( internal() );
	}

	void set_count( int64_t c )
	{
		al_set_timer_count( internal(), c );
	}

	void increment( int64_t d )
	{
		al_add_timer_count( internal(), d );
	}

	ALLEGRO_TIMER *internal( void ) const
	{
		return m_timer.get();
	}

private:
	shared_ptr<ALLEGRO_TIMER> m_timer;
};

////////////////////////////////////////

}

// vim:ft=cpp:
