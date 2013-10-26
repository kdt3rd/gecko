
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

void path::rectangle( const point &p1, const point &p2 )
{
	const double x1 = std::min( p1.x(), p2.x() );
	const double y1 = std::min( p1.y(), p2.y() );
	const double x2 = std::max( p1.x(), p2.x() );
	const double y2 = std::max( p1.y(), p2.y() );

	move_to( { x1, y1 } );
	line_to( { x2, y1 } );
	line_to( { x2, y2 } );
	line_to( { x1, y2 } );
	close();
}

////////////////////////////////////////

void path::rounded_rect( const point &p1, const point &p2, double r )
{
	const double x1 = std::min( p1.x(), p2.x() );
	const double y1 = std::min( p1.y(), p2.y() );
	const double x2 = std::max( p1.x(), p2.x() );
	const double y2 = std::max( p1.y(), p2.y() );

	const double degrees = M_PI / 180.0;

	move_to( { x1 + r, y1 } );
	arc_to( { x2 - r, y1 + r }, r, -90.0 * degrees, 0.0 * degrees );
	arc_to( { x2 - r, y2 - r }, r, 0.0 * degrees, 90.0 * degrees );
	arc_to( { x1 + r, y2 - r }, r, 90.0 * degrees, 180.0 * degrees );
	arc_to( { x1 + r, y1 + r }, r, 180.0 * degrees, 270.0 * degrees );
	close();
}

////////////////////////////////////////

void path::rounded_rect( const point &p1, double w, double h, double r )
{
	rounded_rect( p1, { p1.x() + w - 1.0, p1.y() + h - 1.0 }, r );
}

////////////////////////////////////////

}

