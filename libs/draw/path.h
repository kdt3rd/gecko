//
// Copyright (c) 2014-2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <vector>
#include "types.h"

namespace draw
{

////////////////////////////////////////

/// @brief Path used to draw.
class path
{
public:
	using value_type = dim::value_type;
	static_assert( std::is_floating_point<value_type>::value, "expect floating point dimension type" );
	/// @brief Actions the path can take.
	enum class action
	{
		MOVE,
		LINE,
		QUADRATIC,
		CUBIC,
		ARC,
		CLOSE
	};

	/// @brief Default constructor.
	path( void );

	/// @brief Constructor with initial point.
	path( const point &p );

	path( const path &p ) = default;
	path &operator=( const path &p ) = default;
	path( path &&p ) = default;
	path &operator=( path &&p ) = default;

	/// @brief Destructor.
	~path( void );

	/// @brief Move the cursor.
	/// Move the cursor to the given position.
	/// @param p the point to move to for the path.
	void move_to( const point &p );

	/// @brief Line from the current position to p.
	void line_to( const point &p );

	void horizontal_to( dim x )
	{
		line_to( { x, _last[1] } );
	}

	void vertical_to( dim y )
	{
		line_to( { _last[0], y } );
	}

	/// @brief Quadratic curve from the current position to p2.
	void quadratic_to( const point &p1, const point &p2 );

	/// @brief Cubic curve from the current position to p3.
	void cubic_to( const point &p1, const point &p2, const point &p3 );

	void cubic_to( const point &p2, const point &p3 );

//	void arc_to( const point &p1, const point &p2, dim r );

	/// @brief Arc from the current point sweeping through the angles.
	void arc_to( const point &center, dim radius, float angle1, float angle2 );

	/// @brief Move by a delta.
	void move_by( const point &p ) { move_to( next_point( p ) ); }

	/// @brief Line by a delta.
	void line_by( const point &p ) { line_to( next_point( p ) ); }

	void horizontal_by( dim x )
	{
		line_by( { x, 0 } );
	}

	void vertical_by( dim y )
	{
		line_by( { 0, y } );
	}

	/// @brief Quadratic curve relative to current position.
	void quadratic_by( const point &p1, const point &p2 ) { quadratic_to( next_point( p1 ), next_point( p2 ) ); }

	/// @brief Cubic curve relative to current position.
	void cubic_by( const point &p1, const point &p2, const point &p3 ) { cubic_to( next_point( p1 ), next_point( p2 ), next_point( p3 ) ); }
	void cubic_by( const point &p2, const point &p3 ) { cubic_to( next_point( p2 ), next_point( p3 ) ); }

//	void arc_by( const point &p1, const point &p2, dim r ) { arc_to( next_point( p1 ), next_point( p2 ), r ); }

	/// @brief Arc with center relative to current position.
	void arc_by( const point &center, dim radius, float angle1, float angle2 ) { arc_to( next_point( center ), radius, angle1, angle2 ); }

	/// @brief Close the current shape.
	/// Close the current shape by drawing a line to the starting point.
	void close( void );

	/// @brief Draw a full circle.
	void circle( const point &center, dim radius );

	/// @brief Draw a rectangle.
	void rectangle( const point &p1, const point &p2 );

	/// @brief Draw a rectangle.
	void rectangle( const point &p1, dim w, dim h ) { rectangle( p1, p1 + point( w, h ) ); }

	/// @brief Draw a rounded rectangle.
	void rounded_rect( const point &p1, const point &p2, dim r );

	/// @brief Draw a rounded rectangle.
	void rounded_rect( const point &p1, dim w, dim h, dim r );

	const std::vector<value_type> &get_data( void ) const { return _data; }
	const std::vector<action> &get_actions( void ) const { return _actions; }

	template<typename P>
	void replay( P &player ) const
	{
		auto data = _data.begin();

		point p1, p2, p3;
		value_type x, y, z;

		for ( auto act: _actions )
		{
			switch ( act )
			{
				case action::MOVE:
					player.move_to( pt( data ) );
					break;

				case action::LINE:
					player.line_to( pt( data ) );
					break;

				case action::QUADRATIC:
					p1 = pt( data );
					p2 = pt( data );
					player.quadratic_to( p1, p2 );
					break;

				case action::CUBIC:
					p1 = pt( data );
					p2 = pt( data );
					p3 = pt( data );
					player.cubic_to( p1, p2, p3 );
					break;

				case action::ARC:
					p1 = pt( data );
					x = dbl( data );
					y = dbl( data );
					z = dbl( data );
					player.arc_to( p1, dim( x ), y, z );
					break;

				case action::CLOSE:
					player.close();
					break;
			}
		}
	}

	/// @brief Clear the path.
	void clear( void )
	{
		_start = 0;
		_last = point();
		_data.clear();
		_actions.clear();
	}

	bool empty( void ) const
	{
		return _actions.empty();
	}

private:
	template<typename Iterator>
	point pt( Iterator &i ) const
	{
		dim x = *i++;
		dim y = *i++;
		return point( x, y );
	}

	template<typename Iterator>
	value_type dbl( Iterator &i ) const
	{
		return *i++;
	}

	inline point next_point( const point &d )
	{
		return d + _last;
	}

	void add( void )
	{
	}

	template<typename Head, typename ...Tail>
	inline void add( Head head, Tail && ...tail )
	{
		addit( head );
		add( std::forward<Tail>( tail )... );
	}

	void addit( const point &p )
	{
		addit( p[0] );
		addit( p[1] );
	}

	void addit( dim v )
	{
		_data.push_back( v.count() );
	}

	std::size_t _start = 0;
	point _last = { dim(0), dim(0) };
	point _last2 = { dim(0), dim(0) };
	std::vector<value_type> _data;
	std::vector<action> _actions;
};

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const path &p );

////////////////////////////////////////

}
