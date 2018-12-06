//
// Copyright (c) 2016 Ian Godin
// SPDX-License-Identifier: MIT
//

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

path::path( const point &p )
{
	move_to( p );
}

////////////////////////////////////////

path::~path( void )
{
}

////////////////////////////////////////

void path::move_to( const point &p )
{
	add( p );
	_last = p;
	_actions.push_back( action::MOVE );
}

////////////////////////////////////////

void path::line_to( const point &p )
{
	add( p );
	_last = p;
	_actions.push_back( action::LINE );
}

////////////////////////////////////////

void path::quadratic_to( const point &p1, const point &p2 )
{
	add( p1, p2 );
	_last = p2;
	_actions.push_back( action::QUADRATIC );
}

////////////////////////////////////////

void path::cubic_to( const point &p1, const point &p2, const point &p3 )
{
	add( p1, p2, p3 );
	_last2 = p2;
	_last = p3;
	_actions.push_back( action::CUBIC );
}

////////////////////////////////////////

void path::cubic_to( const point &p2, const point &p3 )
{
	point p1 = _last;
	if ( !_actions.empty() && _actions.back() == action::CUBIC )
		p1 = _last * 2.F - _last2;
	cubic_to( p1, p2, p3 );
}

////////////////////////////////////////

void path::arc_to( const point &center, dim radius, float angle1, float angle2 )
{
	add( center, radius, angle1, angle2 );
	_last = center + base::polar( radius, angle2 );
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

void path::circle( const point &center, dim radius )
{
	move_to( { center[0] + radius, center[1] } );
	arc_to( center, radius, 0.F, 360.0_degf );
	close();
}

////////////////////////////////////////

void path::rectangle( const point &p1, const point &p2 )
{
	const dim x1 = std::min( p1[0], p2[0] );
	const dim y1 = std::min( p1[1], p2[1] );
	const dim x2 = std::max( p1[0], p2[0] );
	const dim y2 = std::max( p1[1], p2[1] );

	move_to( { x1, y1 } );
	line_to( { x2, y1 } );
	line_to( { x2, y2 } );
	line_to( { x1, y2 } );
	close();
}

////////////////////////////////////////

void path::rounded_rect( const point &p1, const point &p2, dim r )
{
	const dim x1 = std::min( p1[0], p2[0] );
	const dim y1 = std::min( p1[1], p2[1] );
	const dim x2 = std::max( p1[0], p2[0] );
	const dim y2 = std::max( p1[1], p2[1] );

	move_to( { x1 + r, y1 } );
	arc_to( { x2 - r, y1 + r }, r, -90.0_degf, 0.0_degf );
	arc_to( { x2 - r, y2 - r }, r, 0.0_degf, 90.0_degf );
	arc_to( { x1 + r, y2 - r }, r, 90.0_degf, 180.0_degf );
	arc_to( { x1 + r, y1 + r }, r, 180.0_degf, 270.0_degf );
	close();
}

////////////////////////////////////////

void path::rounded_rect( const point &p1, dim w, dim h, dim r )
{
	rounded_rect( p1, { p1[0] + w, p1[1] + h }, r );
}

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

		void move_to( const point &x )
		{
			out << "Move( " << x << " )\n";
		}

		void line_to( const point &x )
		{
			out << "Line( " << x << " )\n";
		}

		void quadratic_to( const point &p1, const point &p2 )
		{
			out << "Quadratic( " << p1 << ", " << p2 << " )\n";
		}

		void cubic_to( const point &p1, const point &p2, const point &p3 )
		{
			out << "Cubic( " << p1 << ", " << p2 << ", " << p3 << " )\n";
		}

		void arc_to( const point &center, dim radius, float angle1, float angle2 )
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
