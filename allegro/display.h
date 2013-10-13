
#pragma once

#include <string>
#include <memory>

#include "callegro.h"

namespace allegro
{

////////////////////////////////////////

class display
{
public:
	display( int w, int h )
		: m_display( callegro::al_create_display( w, h ), callegro::al_destroy_display )
	{
	}

	display( const display &d )
		: m_display( d.m_display )
	{
	}

	int width( void ) const
	{
		return callegro::al_get_display_width( m_display.get() );
	}

	int height( void ) const
	{
		return callegro::al_get_display_height( m_display.get() );
	}

	void resize( int w, int h )
	{
		callegro::al_resize_display( m_display.get(), w, h );
	}

	void set_title( const char *title )
	{
		callegro::al_set_window_title( m_display.get(), title );
	}

	void set_title( const std::string &title )
	{
		callegro::al_set_window_title( m_display.get(), title.c_str() );
	}

	void show_cursor( void )
	{
		callegro::al_show_mouse_cursor( internal() );
	}

	void hide_cursor( void )
	{
		callegro::al_hide_mouse_cursor( internal() );
	}

	bool warp_mouse( int x, int y )
	{
		return callegro::al_set_mouse_xy( internal(), x, y );
	}

	callegro::ALLEGRO_DISPLAY *internal( void ) const
	{
		return m_display.get();
	}

private:
	std::shared_ptr<callegro::ALLEGRO_DISPLAY> m_display;
};

////////////////////////////////////////

}

// vim:ft=cpp:
