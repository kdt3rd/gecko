
#include <iostream>
#include "path.h"
#include "geometry.h"
#include <core/contract.h>

namespace 
{
	constexpr double PI = 3.14159265358979323846;
}

namespace gldraw
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
	if ( _lines.empty() )
		_lines.emplace_back();
	if ( !_lines.back().empty() )
		_lines.emplace_back();

	_lines.back().push_back( p );
}

////////////////////////////////////////

void path::line_to( const point &p )
{
	precondition( !_lines.empty(), "no point to start from" );
	precondition( !_lines.back().empty(), "no point to start from" );
	_lines.back().push_back( p );
}

////////////////////////////////////////

void path::quadratic_to( const point &p1, const point &p2 )
{
	precondition( !_lines.empty(), "no point to start from" );
	precondition( !_lines.back().empty(), "no point to start from" );
	add_quadratic( _lines.back().back(), p1, p2, _lines.back() );
}

////////////////////////////////////////

void path::cubic_to( const point &p1, const point &p2, const point &p3 )
{
	precondition( !_lines.empty(), "no point to start from" );
	precondition( !_lines.back().empty(), "no point to start from" );
	add_cubic( _lines.back().back(), p1, p2, p3, _lines.back() );
}

////////////////////////////////////////

/*
void path::arc_to( const point &center, const point &radius, double angle1, double angle2 )
{
	_points.push_back( center );
	_points.push_back( radius );
	_points.emplace_back( angle1, angle2 );
	_actions.push_back( action::ARC );
}
*/

////////////////////////////////////////

void path::close( void )
{
	precondition( !_lines.empty(), "no path to close" );
	_lines.back().close();
	_lines.emplace_back();
}

////////////////////////////////////////

point path::abs_point( const point &d ) const
{
	precondition( !_lines.empty(), "no point to start from" );
	precondition( !_lines.back().empty(), "no point to start from" );
	return _lines.back().back() + d;
}

////////////////////////////////////////

}

