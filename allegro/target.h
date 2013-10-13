
#pragma once

#include "callegro.h"
#include "bitmap.h"
#include "display.h"
#include "font.h"

namespace allegro
{

////////////////////////////////////////

class target
{
public:
	target( const bitmap &bm )
	{
		m_old_target = callegro::al_get_target_bitmap();
		callegro::al_set_target_bitmap( bm.internal() );
	}

	target( const display &d )
	{
		m_old_target = callegro::al_get_target_bitmap();
		callegro::al_set_target_backbuffer( d.internal() );
	}

	~target( void )
	{
		if ( m_old_target )
			callegro::al_set_target_bitmap( m_old_target );
	}

	void set_clipping( int x, int y, int w, int h ) { callegro::al_set_clipping_rectangle( x, y, w, h ); }
	void reset_clipping( void ) { callegro::al_reset_clipping_rectangle(); }

	void clear( const color &c ) { callegro::al_clear_to_color( c.internal() ); }
	
	void draw( const bitmap &bm, float x, float y, bool flip_h = false, bool flip_v = false ) const { callegro::al_draw_bitmap( bm.internal(), x, y, flip_flag( flip_h, flip_v ) ); }
	void draw( const bitmap &bm, const rectangle &dst, bool flip_h = false, bool flip_v = false ) const { const rectangle r( bm.rect() ); callegro::al_draw_scaled_bitmap( bm.internal(), r.x(), r.y(), r.w(), r.h(), dst.x(), dst.y(), dst.w(), dst.h(), flip_flag( flip_h, flip_v ) ); }
	void draw( float x, float y, const color &c ) const { callegro::al_draw_pixel( x, y, c.internal() ); }

	void draw( const std::string &str, const font &f, float x, float y, const color &c ) const { callegro::al_draw_text( f.internal(), c.internal(), x, y, 0, str.c_str() ); }

	void draw_tinted( const bitmap &bm, const color &c, const rectangle &dst, bool flip_h = false, bool flip_v = false ) const
	{
		const rectangle r( bm.rect() );
		callegro::al_draw_tinted_scaled_bitmap( bm.internal(), c.internal(), r.x(), r.y(), r.w(), r.h(), dst.x(), dst.y(), dst.w(), dst.h(), flip_flag( flip_h, flip_v ) );
	}

	void draw_rect( float x1, float y1, float x2, float y2, const color &c, float thick = 1.F ) const { callegro::al_draw_rectangle( x1, y1, x2, y2, c.internal(), thick ); }

	void put_pixel( int x, int y, const color &c ) const { callegro::al_put_pixel( x, y, c.internal() ); }

	void flip( void ) const { callegro::al_flip_display(); }

private:
	inline int flip_flag( bool flip_h, bool flip_v ) const { return ( flip_h ? callegro::ALLEGRO_FLIP_HORIZONTAL : 0 ) | ( flip_v ? callegro::ALLEGRO_FLIP_VERTICAL : 0 ); }

	callegro::ALLEGRO_BITMAP *m_old_target;
};

////////////////////////////////////////

}
// vim:ft=cpp:
