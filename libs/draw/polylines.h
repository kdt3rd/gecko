
#pragma once

#include <vector>
#include "polyline.h"
#include "mesh.h"
#include <core/point.h>

namespace draw
{

////////////////////////////////////////

/// @brief Create polylines
class polylines
{
public:
	/// @brief Constructor
	polylines( void );

	void new_polyline( void );

	/// @brief Move the cursor
	/// Move the cursor to the given position
	/// @param p
	void move_to( const core::point &p );

	void line_to( const core::point &p );

	void quadratic_to( const core::point &p1, const core::point &p2 );

	void cubic_to( const core::point &p1, const core::point &p2, const core::point &p3 );

	void arc_to( const core::point &center, double radius, double angle1, double angle2 );

	void add_point( const core::point &p );

	void close( void );

	polylines stroked( double width );
	polylines offset( double width );

	mesh<core::point> debug( void );
	mesh<core::point> filled( void );

	std::vector<polyline>::iterator begin( void )
	{
		return _lines.begin();
	}

	std::vector<polyline>::const_iterator begin( void ) const
	{
		return _lines.begin();
	}

	std::vector<polyline>::iterator end( void )
	{
		return _lines.end();
	}

	std::vector<polyline>::const_iterator end( void ) const
	{
		return _lines.end();
	}

private:
	std::vector<polyline> _lines;
};

////////////////////////////////////////

inline std::ostream &operator<<( std::ostream &out, const polylines &p )
{
	std::copy( p.begin(), p.end(), std::ostream_iterator<polyline>( out, "\n" ) );
	return out;
}

////////////////////////////////////////

}

