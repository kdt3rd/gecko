
#pragma once

#include <vector>
#include "point.h"

namespace draw
{

////////////////////////////////////////

class path
{
public:
	enum class verb
	{
		MOVE,
		LINE,
		QUADRATIC,
		CUBIC,
		ARC,
		CLOSE
	};

	path( void );
	~path( void );

	void move_to( const point &p );
	void line_to( const point &p );
	void quadratic_to( const point &p1, const point &p2 );
//	void conic_to( const point &p1, const point &p2, double w );
	void cubic_to( const point &p1, const point &p2, const point &p3 );
//	virtual void arc_to( const point &p1, const &point &p2, double r );
	void arc_to( const point &center, const point &radius, double angle1, double angle2 );

	void move_by( const point &p ) { move_to( next_point( p ) ); }
	void line_by( const point &p ) { line_to( next_point( p ) ); }
	void quadratic_by( const point &p1, const point &p2 ) { quadratic_to( next_point( p1 ), next_point( p2 ) ); }
//	void conic_by( const point &p1, const point &p2, double w ) { conic_to( next_point( p1 ), next_point( p2 ), w ); };
	void cubic_by( const point &p1, const point &p2, const point &p3 ) { cubic_to( next_point( p1 ), next_point( p2 ), next_point( p3 ) ); }
//	void arc_by( const point &p1, const &point &p2, double r ) { arc_to( next_point( p1 ), next_point( p2 ), r ); }
	void arc_by( const point &center, const point &radius, double angle1, double angle2 ) { arc_to( next_point( center ), radius, angle1, angle2 ); }

	void close( void );

	const std::vector<point> &get_points( void ) const { return _points; }
	const std::vector<verb> &get_verbs( void ) const { return _verbs; }

private:
	point next_point( const point &d );

	std::size_t _start = 0;
	std::vector<point> _points;
	std::vector<verb> _verbs;
};

////////////////////////////////////////

}

