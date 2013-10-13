
#pragma once

#include "callegro.h"
#include <cstdint>

namespace allegro
{

////////////////////////////////////////

class color
{
public:
	color( callegro::ALLEGRO_COLOR c )
		: m_color( c )
	{
	}

	/*
	color( float g )
		: m_color( al_map_rgb_f( g, g, g ) )
	{
	}

	color( float r, float g, float b )
		: m_color( al_map_rgb_f( r, g, b ) )
	{
	}
	*/

	color( uint8_t r, uint8_t g, uint8_t b )
		: m_color( callegro::al_map_rgb( r, g, b ) )
	{
	}

	color( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
		: m_color( callegro::al_map_rgba( r, g, b, a ) )
	{
	}

	callegro::ALLEGRO_COLOR internal( void ) const { return m_color; }

protected:
	callegro::ALLEGRO_COLOR m_color;
};

////////////////////////////////////////

}
// vim:ft=cpp:
