
#include <iostream>
#include "path.h"

namespace 
{
	constexpr double PI = 3.14159265358979323846;
}

namespace core
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
	_last = p3;
	_actions.push_back( action::CUBIC );
}

////////////////////////////////////////

void path::arc_to( const point &center, double radius, double angle1, double angle2 )
{
	add( center, radius, angle1, angle2 );
	_last = center + point::polar( radius, angle2 );
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

void path::circle( const point &center, double radius )
{
	move_to( { center.x() + radius, center.y() } );
	arc_to( center, radius, 0.0, PI * 2.0 );
	close();
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
	const double x2 = std::max( p1.x(), p2.x() ) - 1.0;
	const double y2 = std::max( p1.y(), p2.y() ) - 1.0;

	const double degrees = PI / 180.0;

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

std::ostream &operator<<( std::ostream &out, const path &p )
{
	class printer
	{
	public:
		printer( std::ostream &o )
			: out( o )
		{
		}

		void move_to( const point &p )
		{
			out << "Move( " << p << " )\n";
		}

		void line_to( const point &p )
		{
			out << "Line( " << p << " )\n";
		}

		void quadratic_to( const point &p1, const point &p2 )
		{
			out << "Quadratic( " << p1 << ", " << p2 << " )\n";
		}

		void cubic_to( const point &p1, const point &p2, const point &p3 )
		{
			out << "Cubic( " << p1 << ", " << p2 << ", " << p3 << " )\n";
		}

		void arc_to( const point &center, double radius, double angle1, double angle2 )
		{
			out << "Arc( " << center << ", " << radius << ", " << angle1 * 180 / PI << ", " << angle2 * 180 / PI << " )\n";
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

