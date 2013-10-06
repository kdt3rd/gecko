
#pragma once

#include <stdexcept>
#include <string>
#include <memory>
extern "C" {
#include <allegro5/allegro.h>
}

#include "rectangle.h"
#include "color.h"

using namespace std;

namespace allegro
{

////////////////////////////////////////

class bitmap
{
public:
	bitmap( void )
	{
	}

	bitmap( int w, int h )
		: m_bitmap( al_create_bitmap( w, h ), al_destroy_bitmap )
	{
	}

	bitmap( const bitmap &bitmap )
		: m_bitmap( bitmap.m_bitmap )
	{
	}

	bitmap( const bitmap &bitmap, int x, int y, int w, int h )
		: m_bitmap( al_create_sub_bitmap( bitmap.internal(), x, y, w, h ), al_destroy_bitmap )
	{
	}

	bitmap( const char *filename )
		: m_bitmap( al_load_bitmap( filename ), al_destroy_bitmap )
	{
		if ( !m_bitmap )
			throw std::runtime_error( "error loading bitmap" );
	}

	bitmap( const std::string &filename )
		: m_bitmap( al_load_bitmap( filename.c_str() ), al_destroy_bitmap )
	{
		if ( !m_bitmap )
			throw std::runtime_error( "error loading bitmap" );
	}

	inline void load( const char *filename )
	{
		m_bitmap = shared_ptr<ALLEGRO_BITMAP>( al_load_bitmap( filename ), al_destroy_bitmap );
		if ( !m_bitmap )
			throw std::runtime_error( "error loading bitmap" );
	}

	inline void load( const std::string &filename )
	{
		load( filename.c_str() );
	}

	inline void save( const char *filename )
	{
		al_save_bitmap( filename, internal() );
	}

	inline void save( const std::string &filename )
	{
		save( filename.c_str() );
	}

	inline int width( void ) const
	{
		return al_get_bitmap_width( internal() );
	}

	inline int height( void ) const
	{
		return al_get_bitmap_height( internal() );
	}

	inline color pixel( int x, int y ) const
	{
		return al_get_pixel( internal(), x, y );
	}

	inline bitmap clone( void ) const
	{
		return bitmap( al_clone_bitmap( internal() ) );
	}

	inline ALLEGRO_BITMAP *internal( void ) const
	{
		return m_bitmap.get();
	}

	inline rectangle rect( void ) const
	{
		return rectangle( 0, 0, width(), height() );
	}

protected:
	bitmap( ALLEGRO_BITMAP *bm )
		: m_bitmap( bm, al_destroy_bitmap )
	{
	}

	shared_ptr<ALLEGRO_BITMAP> m_bitmap;
};

////////////////////////////////////////

}
// vim:ft=cpp:
