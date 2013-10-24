
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

void path::rounded_rect( const point &p1, const point &p2, double r )
{
	const double x = std::min( p1.x(), p2.x() );
	const double y = std::min( p1.y(), p2.y() );
	const double w = std::abs( p2.x() - p1.x() );
	const double h = std::abs( p2.y() - p1.y() );
	rounded_rect( { x, y }, w, h, r );
}

////////////////////////////////////////

void path::rounded_rect( const point &p1, double w, double h, double r )
{
	const double x = p1.x();
	const double y = p1.y();

	const double degrees = M_PI / 180.0;

	arc_to( { x + w - r, y + r }, r, -90.0 * degrees, 0.0 * degrees );
	arc_to( { x + w - r, y + h - r }, r, 0.0 * degrees, 90.0 * degrees );
	arc_to( { x + r, y + h - r }, r, 90.0 * degrees, 180.0 * degrees );
	arc_to( { x + r, y + r }, r, 180.0 * degrees, 270.0 * degrees );
	close();
}

////////////////////////////////////////

}

