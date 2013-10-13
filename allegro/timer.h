
#pragma once

#include "callegro.h"

namespace allegro
{

////////////////////////////////////////

class timer
{
public:
	timer( double secs )
		: m_timer( callegro::al_create_timer( secs ), callegro::al_destroy_timer )
	{
	}

	double timer_speed( void ) const
	{
		return callegro::al_get_timer_speed( internal() );
	}

	void set_timer_speed( double secs ) const
	{
		return callegro::al_set_timer_speed( internal(), secs );
	}

	void start( void )
	{
		callegro::al_start_timer( internal() );
	}

	void stop( void )
	{
		callegro::al_stop_timer( internal() );
	}

	bool started( void ) const
	{
		return callegro::al_get_timer_started( internal() );
	}

	int64_t count( void ) const
	{
		return callegro::al_get_timer_count( internal() );
	}

	void set_count( int64_t c )
	{
		callegro::al_set_timer_count( internal(), c );
	}

	void increment( int64_t d )
	{
		callegro::al_add_timer_count( internal(), d );
	}

	callegro::ALLEGRO_TIMER *internal( void ) const
	{
		return m_timer.get();
	}

private:
	std::shared_ptr<callegro::ALLEGRO_TIMER> m_timer;
};

////////////////////////////////////////

}

// vim:ft=cpp:
