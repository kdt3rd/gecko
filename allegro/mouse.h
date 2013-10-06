
#pragma once

extern "C" {
#include <allegro5/allegro.h>
}

#include <stdexcept>

namespace allegro
{

////////////////////////////////////////

class mouse
{
public:
	mouse( void )
	{
		if ( al_is_mouse_installed() )
			throw std::runtime_error( "only one mouse allowed" );

		if ( !al_install_mouse() )
			throw std::runtime_error( "error installing mouse" );
	}

	~mouse( void )
	{
		al_uninstall_mouse();
	}

	int num_axes( void )
	{
		return al_get_mouse_num_axes();
	}

	int num_buttons( void )
	{
		return al_get_mouse_num_buttons();
	}

	void update_state( void )
	{
		al_get_mouse_state( &m_state );
	}

	int get_axis( int axis )
	{
		return al_get_mouse_state_axis( &m_state, axis );
	}

	bool button_down( int button )
	{
		return al_mouse_button_down( &m_state, button + 1 );
	}

	bool set_z( int z )
	{
		return al_set_mouse_z( z );
	}

	bool set_w( int w )
	{
		return al_set_mouse_w( w );
	}

private:
	ALLEGRO_MOUSE_STATE m_state;
};

////////////////////////////////////////

}

// vim:ft=cpp:
