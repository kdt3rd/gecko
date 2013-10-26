
#pragma once

#include <vector>
#include "point.h"

namespace draw
{

////////////////////////////////////////

/// @brief Path used to draw
class path
{
public:
	/// @brief Actions the path can take
	enum class action
	{
		MOVE,
		LINE,
		QUADRATIC,
		CUBIC,
		ARC,
		CLOSE
	};

	/// @brief Constructor
	path( void );
	/// @brief Destructor
	~path( void );

	/// @brief Move the cursor
	/// Move the cursor to the given position
	/// @param p
	void move_to( const point &p );
	void line_to( const point &p );
	void quadratic_to( const point &p1, const point &p2 );
//	void conic_to( const point &p1, const point &p2, double w );
	void cubic_to( const point &p1, const point &p2, const point &p3 );
//	virtual void arc_to( const point &p1, const &point &p2, double r );
	void arc_to( const point &center, const point &radius, double angle1, double angle2 );
	void arc_to( const point &center, float radius, double angle1, double angle2 ) { arc_to( center, { center.x() + radius, center.y() }, angle1, angle2 ); }

	void move_by( const point &p ) { move_to( next_point( p ) ); }
	void line_by( const point &p ) { line_to( next_point( p ) ); }
	void quadratic_by( const point &p1, const point &p2 ) { quadratic_to( next_point( p1 ), next_point( p2 ) ); }
//	void conic_by( const point &p1, const point &p2, double w ) { conic_to( next_point( p1 ), next_point( p2 ), w ); };
	void cubic_by( const point &p1, const point &p2, const point &p3 ) { cubic_to( next_point( p1 ), next_point( p2 ), next_point( p3 ) ); }
//	void arc_by( const point &p1, const &point &p2, double r ) { arc_to( next_point( p1 ), next_point( p2 ), r ); }
	void arc_by( const point &center, const point &radius, double angle1, double angle2 ) { arc_to( next_point( center ), radius, angle1, angle2 ); }

	void close( void );

	void rectangle( const point &p1, const point &p2 );

	void rounded_rect( const point &p1, const point &p2, double r );
	void rounded_rect( const point &p1, double w, double h, double r );

	const point &get_point( std::size_t i ) const { return _points.at( i ); }
	const std::vector<point> &get_points( void ) const { return _points; }
	const std::vector<action> &get_actions( void ) const { return _actions; }

private:
	point next_point( const point &d );

	std::size_t _start = 0;
	std::vector<point> _points;
	std::vector<action> _actions;
};

////////////////////////////////////////

}

