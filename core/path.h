
#pragma once

#include <vector>
#include "point.h"

namespace core
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

	void rectangle( const point &p1, const point &p2 );

	void rounded_rect( const point &p1, const point &p2, double r );
	void rounded_rect( const point &p1, double w, double h, double r );

	const std::vector<double> &get_data( void ) const { return _data; }
	const std::vector<action> &get_actions( void ) const { return _actions; }

	template<typename P>
	void replay( P &player ) const
	{
		auto data = _data.begin();
		auto pt = [&]() { double x = *data++; double y = *data++; return core::point( x, y ); };
		auto dbl = [&]() { return *data++; };

		for ( auto act: _actions )
		{
			switch ( act )
			{
				case action::MOVE: player.move_to( pt() ); break;
				case action::LINE: player.line_to( pt() ); break;
				case action::QUADRATIC: player.quadratic_to( pt(), pt() ); break;
				case action::CUBIC: player.cubic_to( pt(), pt(), pt() ); break;
				case action::ARC: player.arc_to( pt(), dbl(), dbl(), dbl() ); break;
				case action::CLOSE: player.close();
			}
		}
	}

private:
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
		add( p.x() );
		add( p.y() );
	}

	void addit( double v )
	{
		_data.push_back( v );
	}

	std::size_t _start = 0;
	core::point _last = { 0, 0 };
	std::vector<double> _data;
	std::vector<action> _actions;
};

////////////////////////////////////////

std::ostream &operator<<( std::ostream &out, const path &p );

////////////////////////////////////////

}

