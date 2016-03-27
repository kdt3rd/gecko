
#include <iostream>
#include <base/math_functions.h>
#include "path.h"

namespace draw
{

////////////////////////////////////////

path::path( void )
{
}

////////////////////////////////////////

path::path( const gl::vec2 &p )
{
	move_to( p );
}

////////////////////////////////////////

/*
path::path( const rect &r )
{
	rectangle( r );
}
*/

////////////////////////////////////////

/*
path::path( const rect &r, float rad )
{
	rounded_rect( r, rad );
}
*/

////////////////////////////////////////

path::~path( void )
{
}

////////////////////////////////////////

void path::move_to( const gl::vec2 &p )
{
	add( p );
	_last = p;
	_actions.push_back( action::MOVE );
}

////////////////////////////////////////

void path::line_to( const gl::vec2 &p )
{
	add( p );
	_last = p;
	_actions.push_back( action::LINE );
}

////////////////////////////////////////

void path::quadratic_to( const gl::vec2 &p1, const gl::vec2 &p2 )
{
	add( p1, p2 );
	_last = p2;
	_actions.push_back( action::QUADRATIC );
}

////////////////////////////////////////

void path::cubic_to( const gl::vec2 &p1, const gl::vec2 &p2, const gl::vec2 &p3 )
{
	add( p1, p2, p3 );
	_last = p3;
	_actions.push_back( action::CUBIC );
}

////////////////////////////////////////

void path::arc_to( const gl::vec2 &center, float radius, float angle1, float angle2 )
{
	add( center, radius, angle1, angle2 );
	_last = center + gl::vec2::polar( radius, angle2 );
	_actions.push_back( action::ARC );
}

////////////////////////////////////////

void path::close( void )
{
	if ( _start < _actions.size() )
	{
		_actions.push_back( action::CLOSE );
		_start = _actions.size();
	}
}

////////////////////////////////////////

void path::circle( const gl::vec2 &center, float radius )
{
	move_to( { center[0] + radius, center[1] } );
	arc_to( center, radius, 0.F, base::degree( 360.F ) );
	close();
}

////////////////////////////////////////

/*
void path::rectangle( const rect &r, bool reversed )
{
	if ( reversed )
	{
		move_to( r.top_left() );
		line_to( r.bottom_left() );
		line_to( r.bottom_right() );
		line_to( r.top_right() );
	}
	else
	{
		move_to( r.top_left() );
		line_to( r.top_right() );
		line_to( r.bottom_right() );
		line_to( r.bottom_left() );
	}
	close();
}
*/

////////////////////////////////////////

void path::rectangle( const gl::vec2 &p1, const gl::vec2 &p2 )
{
	const float x1 = std::min( p1[0], p2[0] );
	const float y1 = std::min( p1[1], p2[1] );
	const float x2 = std::max( p1[0], p2[0] );
	const float y2 = std::max( p1[1], p2[1] );

	move_to( { x1, y1 } );
	line_to( { x2, y1 } );
	line_to( { x2, y2 } );
	line_to( { x1, y2 } );
	close();
}

////////////////////////////////////////

void path::rounded_rect( const gl::vec2 &p1, const gl::vec2 &p2, float r )
{
	const float x1 = std::min( p1[0], p2[0] );
	const float y1 = std::min( p1[1], p2[1] );
	const float x2 = std::max( p1[0], p2[0] );
	const float y2 = std::max( p1[1], p2[1] );

	move_to( { x1 + r, y1 } );
	arc_to( { x2 - r, y1 + r }, r, base::degree( -90.F ), base::degree( 0.F ) );
	arc_to( { x2 - r, y2 - r }, r, base::degree( 0.F ), base::degree( 90.F ) );
	arc_to( { x1 + r, y2 - r }, r, base::degree( 90.F ), base::degree( 180.F ) );
	arc_to( { x1 + r, y1 + r }, r, base::degree( 180.F ), base::degree( 270.F ) );
	close();
}

////////////////////////////////////////

void path::rounded_rect( const gl::vec2 &p1, float w, float h, float r )
{
	rounded_rect( p1, { p1[0] + w, p1[1] + h }, r );
}

////////////////////////////////////////

/*
void path::rounded_rect( const rect &r, float rad )
{
	rounded_rect( r.top_left(), r.bottom_right(), rad );
}
*/

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const path &p )
{
	class printer
	{
	public:
		printer( std::ostream &o )
			: out( o )
		{
		}

		void move_to( const gl::vec2 &x )
		{
			out << "Move( " << x << " )\n";
		}

		void line_to( const gl::vec2 &x )
		{
			out << "Line( " << x << " )\n";
		}

		void quadratic_to( const gl::vec2 &p1, const gl::vec2 &p2 )
		{
			out << "Quadratic( " << p1 << ", " << p2 << " )\n";
		}

		void cubic_to( const gl::vec2 &p1, const gl::vec2 &p2, const gl::vec2 &p3 )
		{
			out << "Cubic( " << p1 << ", " << p2 << ", " << p3 << " )\n";
		}

		void arc_to( const gl::vec2 &center, float radius, float angle1, float angle2 )
		{
			out << "Arc( " << center << ", " << radius << ", " << angle1 << ", " << angle2 << " )\n";
		}

		void close( void )
		{
			out << "Close()\n";
		}

	private:
		std::ostream &out;
	};

	printer show( out );
	p.replay( show );

	return out;
}

////////////////////////////////////////

}

