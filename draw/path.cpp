
#include "path.h"

namespace draw
{

////////////////////////////////////////

path::path( void )
{
}

////////////////////////////////////////

path::~path( void )
{
}

////////////////////////////////////////

void path::move_to( const point &p )
{
	_points.push_back( p );
	_verbs.push_back( verb::MOVE );
}

////////////////////////////////////////

void path::line_to( const point &p )
{
	_points.push_back( p );
	_verbs.push_back( verb::LINE );
}

////////////////////////////////////////

void path::quadratic_to( const point &p1, const point &p2 )
{
	_points.push_back( p1 );
	_points.push_back( p2 );
	_verbs.push_back( verb::QUADRATIC );
}

////////////////////////////////////////

void path::cubic_to( const point &p1, const point &p2, const point &p3 )
{
	_points.push_back( p1 );
	_points.push_back( p2 );
	_points.push_back( p3 );
	_verbs.push_back( verb::CUBIC );
}

////////////////////////////////////////

void path::arc_to( const point &center, const point &radius, double angle1, double angle2 )
{
	_points.push_back( center );
	_points.push_back( radius );
	_points.emplace_back( angle1, angle2 );
	_verbs.push_back( verb::ARC );
}

////////////////////////////////////////

void path::close( void )
{
	if ( _start < _points.size() )
	{
		_verbs.push_back( verb::CLOSE );
		_start = _points.size();
	}
}

////////////////////////////////////////

}

