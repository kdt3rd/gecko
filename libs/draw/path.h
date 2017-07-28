//
// Copyright (c) 2014-2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <vector>
#include <gl/vector.h>
#include <base/rect.h>

namespace draw
{

////////////////////////////////////////

/// @brief Path used to draw.
class path
{
public:
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
	path( const gl::vec2 &p );

	/// @brief Destructor.
	~path( void );

	/// @brief Move the cursor.
	/// Move the cursor to the given position.
	/// @param p the gl::vec2 to move to for the path.
	void move_to( const gl::vec2 &p );

	/// @brief Line from the current position to p.
	void line_to( const gl::vec2 &p );

	/// @brief Quadratic curve from the current position to p2.
	void quadratic_to( const gl::vec2 &p1, const gl::vec2 &p2 );

	/// @brief Cubic curve from the current position to p3.
	void cubic_to( const gl::vec2 &p1, const gl::vec2 &p2, const gl::vec2 &p3 );

//	void arc_to( const gl::vec2 &p1, const &gl::vec2 &p2, float r );

	/// @brief Arc from the current gl::vec2 sweeping through the angles.
	void arc_to( const gl::vec2 &center, float radius, float angle1, float angle2 );

	/// @brief Move by a delta.
	void move_by( const gl::vec2 &p ) { move_to( next_point( p ) ); }

	/// @brief Line by a delta.
	void line_by( const gl::vec2 &p ) { line_to( next_point( p ) ); }

	/// @brief Quadratic curve relative to current position.
	void quadratic_by( const gl::vec2 &p1, const gl::vec2 &p2 ) { quadratic_to( next_point( p1 ), next_point( p2 ) ); }

	/// @brief Cubic curve relative to current position.
	void cubic_by( const gl::vec2 &p1, const gl::vec2 &p2, const gl::vec2 &p3 ) { cubic_to( next_point( p1 ), next_point( p2 ), next_point( p3 ) ); }

//	void arc_by( const gl::vec2 &p1, const &gl::vec2 &p2, float r ) { arc_to( next_point( p1 ), next_point( p2 ), r ); }

	/// @brief Arc with center relative to current position.
	void arc_by( const gl::vec2 &center, float radius, float angle1, float angle2 ) { arc_to( next_point( center ), radius, angle1, angle2 ); }

	/// @brief Close the current shape.
	/// Close the current shape by drawing a line to the starting point.
	void close( void );

	/// @brief Draw a full circle.
	void circle( const gl::vec2 &center, float radius );

	/// @brief Draw a rectangle.
	void rectangle( const gl::vec2 &p1, const gl::vec2 &p2 );

	/// @brief Draw a rectangle.
	void rectangle( const gl::vec2 &p1, float w, float h ) { rectangle( p1, p1 + gl::vec2( w, h ) ); }

	/// @brief Draw a rounded rectangle.
	void rounded_rect( const gl::vec2 &p1, const gl::vec2 &p2, float r );

	/// @brief Draw a rounded rectangle.
	void rounded_rect( const gl::vec2 &p1, float w, float h, float r );

	const std::vector<float> &get_data( void ) const { return _data; }
	const std::vector<action> &get_actions( void ) const { return _actions; }

	template<typename P>
	void replay( P &player ) const
	{
		auto data = _data.begin();

		gl::vec2 p1, p2, p3;
		float x, y, z;

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
					player.arc_to( p1, x, y, z );
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
		_last = { 0, 0 };
		_data.clear();
		_actions.clear();
	}

	bool empty( void ) const
	{
		return _actions.empty();
	}

private:
	template<typename Iterator>
	gl::vec2 pt( Iterator &i ) const
	{
		float x = *i++;
		float y = *i++;
		return gl::vec2( x, y );
	}

	template<typename Iterator>
	float dbl( Iterator &i ) const
	{
		return *i++;
	}

	inline gl::vec2 next_point( const gl::vec2 &d )
	{
		return d + _last;
	}

	void add( void )
	{
	}

	template<typename Head, typename ...Tail>
	inline void add( Head head, Tail ...tail )
	{
		addit( head );
		add( tail... );
	}

	void addit( const gl::vec2 &p )
	{
		addit( p[0] );
		addit( p[1] );
	}

	void addit( float v )
	{
		_data.push_back( v );
	}

	std::size_t _start = 0;
	gl::vec2 _last = { 0, 0 };
	std::vector<float> _data;
	std::vector<action> _actions;
};

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const path &p );

////////////////////////////////////////

}

