
#pragma once

#include<iostream>
#include "callegro.h"
#include <memory>

namespace allegro
{

////////////////////////////////////////

class font
{
public:
	font( void )
	{
	}

	font( const char *filename, int size )
		: _font( callegro::al_load_ttf_font( filename, size, 0 ), callegro::al_destroy_font )
	{
		std::cout << filename << ' ' << _font << std::endl;
		if ( !_font )
			throw std::runtime_error( "error loading font" );
	}

	inline void load( const char *filename, int size )
	{
		_font = std::shared_ptr<callegro::ALLEGRO_FONT>( callegro::al_load_ttf_font( filename, size, 0 ), callegro::al_destroy_font );
		std::cout << "Font: " << _font << std::endl;
		if ( !_font )
			throw std::runtime_error( "error loading font" );
	}

	inline callegro::ALLEGRO_FONT *internal( void ) const
	{
		return _font.get();
	}

private:
	std::shared_ptr<callegro::ALLEGRO_FONT> _font;
};

////////////////////////////////////////

}

