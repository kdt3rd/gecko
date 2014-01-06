
#pragma once

#include <vector>
#include "polyline.h"
#include "point.h"

namespace gldraw
{

////////////////////////////////////////

/// @brief Path used to draw
class path
{
public:
	/// @brief Constructor
	path( void );

	path( const point &p );

	/// @brief Destructor
	~path( void );

	/// @brief Move the cursor
	/// Move the cursor to the given position
	/// @param p
	void move_to( const point &p );

	void line_to( const point &p );

	void quadratic_to( const point &p1, const point &p2 );

	void cubic_to( const point &p1, const point &p2, const point &p3 );

	/*
	void arc_to( const point &center, double radius, double angle1, double angle2 );
	*/

	void move_by( const point &p )
	{
		move_to( abs_point( p ) );
	}

	void line_by( const point &p )
	{
		line_to( abs_point( p ) );
	}

	/*
	void quadratic_by( const point &p1, const point &p2 )
	{
		quadratic_to( abs_point( p1 ), abs_point( p1 + p2 ) );
	}

	void cubic_by( const point &p1, const point &p2, const point &p3 )
	{
		cubic_to( abs_point( p1 ), abs_point( p1 + p2 ), next_point( p1 + p2 + p3 ) );
	}

	void arc_by( const point &center, const double radius, double angle1, double angle2 )
	{
		arc_to( next_point( center ), radius, angle1, angle2 );
	}
	*/

	void close( void );

//	std::shared_ptr<drawable> stroked( ... );
//	std::shared_ptr<drawable> filled( ... );

	path stroked( double width );

	const std::vector<polyline> lines( void ) const
	{
		return _lines;
	}

private:
	point abs_point( const point &d ) const;

	std::vector<polyline> _lines;
};

////////////////////////////////////////

}

