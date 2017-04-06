//
// Copyright (c) 2014-2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <vector>
#include "polyline.h"
#include <gl/vector.h>

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
	void move_to( const gl::vec2 &p );

	/// @brief Add a line
	/// draw a line to the given position
	/// @param p desired position
	void line_to( const gl::vec2 &p );

	void quadratic_to( const gl::vec2 &p1, const gl::vec2 &p2 );

	void cubic_to( const gl::vec2 &p1, const gl::vec2 &p2, const gl::vec2 &p3 );

	void arc_to( const gl::vec2 &center, float radius, float angle1, float angle2 );

	void add_point( const gl::vec2 &p );

	void close( void );

	polylines stroked( float width );
	polylines offset( float width );

	void filled( const std::function<void(float,float)> &points, const std::function<void(size_t,size_t,size_t)> &tris );

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

