
#pragma once

#include <vector>
#include "point.h"
#include "rect.h"

namespace base
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

	path( const point &p );
	path( const rect &r );

	/// @brief Destructor
	~path( void );

	/// @brief Move the cursor
	/// Move the cursor to the given position
	/// @param p
	void move_to( const point &p );
	void line_to( const point &p );
	void quadratic_to( const point &p1, const point &p2 );
	void cubic_to( const point &p1, const point &p2, const point &p3 );
//	void arc_to( const point &p1, const &point &p2, double r );
	void arc_to( const point &center, double radius, double angle1, double angle2 );

	void move_by( const point &p ) { move_to( next_point( p ) ); }
	void line_by( const point &p ) { line_to( next_point( p ) ); }
	void quadratic_by( const point &p1, const point &p2 ) { quadratic_to( next_point( p1 ), next_point( p2 ) ); }
	void cubic_by( const point &p1, const point &p2, const point &p3 ) { cubic_to( next_point( p1 ), next_point( p2 ), next_point( p3 ) ); }
//	void arc_by( const point &p1, const &point &p2, double r ) { arc_to( next_point( p1 ), next_point( p2 ), r ); }
	void arc_by( const point &center, double radius, double angle1, double angle2 ) { arc_to( next_point( center ), radius, angle1, angle2 ); }

	void close( void );

	void circle( const point &center, double radius );

	void rectangle( const rect &r );
	void rectangle( const point &p1, const point &p2 );
	void rectangle( const point &p1, double w, double h ) { rectangle( p1, p1 + point( w, h ) ); }

	void rounded_rect( const point &p1, const point &p2, double r );
	void rounded_rect( const point &p1, double w, double h, double r );
	void rounded_rect( const rect &r, double rad );

	const std::vector<double> &get_data( void ) const { return _data; }
	const std::vector<action> &get_actions( void ) const { return _actions; }

	template<typename P>
	void replay( P &player ) const
	{
		auto data = _data.begin();

		point p1, p2, p3;
		double x, y, z;

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

	void clear( void )
	{
		_start = 0;
		_last = { 0, 0 };
		_data.clear();
		_actions.clear();
	}

private:
	template<typename Iterator>
	point pt( Iterator &i ) const
	{
		double x = *i++;
		double y = *i++;
		return point( x, y );
	}

	template<typename Iterator>
	double dbl( Iterator &i ) const
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
	inline void add( Head head, Tail ...tail )
	{
		addit( head );
		add( tail... );
	}

	void addit( const point &p )
	{
		addit( p.x() );
		addit( p.y() );
	}

	void addit( double v )
	{
		_data.push_back( v );
	}

	std::size_t _start = 0;
	point _last = { 0, 0 };
	std::vector<double> _data;
	std::vector<action> _actions;
};

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const path &p );

////////////////////////////////////////

}

