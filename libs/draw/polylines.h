
#pragma once

#include <vector>
#include "polyline.h"
//#include "mesh.h"
#include <base/point.h>

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
	/// @param p desired position
	void move_to( const base::point &p );

	/// @brief Add a line
	/// draw a line to the given position
	/// @param p desired position
	void line_to( const base::point &p );

	void quadratic_to( const base::point &p1, const base::point &p2 );

	void cubic_to( const base::point &p1, const base::point &p2, const base::point &p3 );

	void arc_to( const base::point &center, double radius, double angle1, double angle2 );

	void add_point( const base::point &p );

	void close( void );

	polylines stroked( double width );
	polylines offset( double width );

//	mesh<base::point> debug( void );
//	mesh<base::point> filled( void );

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

	void save_svg( std::ostream &out );

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

