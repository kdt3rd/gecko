// Copyright (c) 2014-2016 Ian Godin
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>
#include "polyline.h"
#include <gl/vector.h>
#include <gl/mesh.h>

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
	void move_to( const polyline::point &p );

	/// @brief Add a line
	/// draw a line to the given position
	/// @param p desired position
	void line_to( const polyline::point &p );

	void quadratic_to( const polyline::point &p1, const polyline::point &p2 );

	void cubic_to( const polyline::point &p1, const polyline::point &p2, const polyline::point &p3 );

	void arc_to( const polyline::point &center, dim radius, float angle1, float angle2 );

	void add_point( const polyline::point &p );

	void close( void );

	void push_back( polyline &&p )
	{
		_lines.emplace_back( std::move( p ) );
	}

	size_t size( void )
	{
		return _lines.size();
	}

	const polyline &operator[]( size_t i ) const
	{
		return _lines[i];
	}

	polyline &operator[]( size_t i )
	{
		return _lines[i];
	}

	polylines stroked( dim width, dim dx = dim(0), dim dy = dim(0), dim dz = dim(0) ) const;
	polylines offset( dim width, dim dx = dim(0), dim dy = dim(0), dim dz = dim(0) ) const;

	void filled( const std::function<void(float,float)> &points, const std::function<void(uint32_t,uint32_t,uint32_t)> &tris ) const;
	void filled( gl::mesh &mesh, const std::string &pos ) const;

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

	bool empty( void ) const
	{
		return _lines.empty();
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

/*
polylines operator&( const polylines &a, const polylines &b );
polylines operator|( const polylines &a, const polylines &b );
polylines operator-( const polylines &a, const polylines &b );
polylines operator^( const polylines &a, const polylines &b );
*/

////////////////////////////////////////

}
