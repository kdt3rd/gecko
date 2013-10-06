
#pragma once

#include <string>
#include <memory>

extern "C" {
#include <allegro5/allegro.h>
}

using namespace std;

namespace allegro
{

////////////////////////////////////////

class display
{
public:
	display( int w, int h )
		: m_display( al_create_display( w, h ), al_destroy_display )
	{
	}

	display( const display &d )
		: m_display( d.m_display )
	{
	}

	int width( void ) const
	{
		return al_get_display_width( m_display.get() );
	}

	int height( void ) const
	{
		return al_get_display_height( m_display.get() );
	}

	void resize( int w, int h )
	{
		al_resize_display( m_display.get(), w, h );
	}

	void set_title( const char *title )
	{
		al_set_window_title( m_display.get(), title );
	}

	void set_title( const string &title )
	{
		al_set_window_title( m_display.get(), title.c_str() );
	}

	void show_cursor( void )
	{
		al_show_mouse_cursor( internal() );
	}

	void hide_cursor( void )
	{
		al_hide_mouse_cursor( internal() );
	}

	bool warp_mouse( int x, int y )
	{
		return al_set_mouse_xy( internal(), x, y );
	}

	ALLEGRO_DISPLAY *internal( void ) const
	{
		return m_display.get();
	}

private:
	shared_ptr<ALLEGRO_DISPLAY> m_display;
};

////////////////////////////////////////

}

// vim:ft=cpp:
