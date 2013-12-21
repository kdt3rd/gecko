
#pragma once

#include <draw/rect.h>

namespace layout
{

////////////////////////////////////////

/// @brief Rectangular area
///
/// Represents an area, with a minimum size.  Used by layouts, views, and reactions.
class simple_area
{
public:
	/// @brief Constructor.
	simple_area( void )
	{
	}

	/// @brief Constructor.
	///
	/// Create an area with the given rectangle.
	/// @param r Rectangle to initialize the area with.
	simple_area( const draw::rect &r )
		: _rect( r )
	{
	}

	/// @brief Constructor.
	///
	/// @param p Position of the top-left corner of the area
	/// @param w Width of the area
	/// @param h Height of the area
	simple_area( const draw::point &p, double w, double h )
		: _rect( p, w, h )
	{
	}

	/// @brief Constructor.
	///
	/// @param w Width of the area
	/// @param h Height of the area
	simple_area( double w, double h )
		: _rect( { 0, 0 }, w, h )
	{
	}

	virtual ~simple_area( void );

	/// @brief The rectangle.
	///
	/// The rectangle representing the area.
	const draw::rect &rectangle( void ) const { return _rect; }

	/// @brief Set minimum width.
	///
	/// @param w The minimum width.
	void set_minimum_width( double w ) { _minw = w; }

	/// @brief Set minimum height.
	///
	/// @param h The minimum height.
	void set_minimum_height( double h ) { _minh = h; }

	/// @brief Set minimum size.
	///
	/// @param w The minimum width.
	/// @param h The minimum height.
	void set_minimum( double w, double h ) { _minw = w; _minh = h; }

	/// @brief Minimum width.
	///
	/// The minimum width of the area.
	/// @return The minimum width.
	double minimum_width( void ) const { return _minw; }

	/// @brief Minimum height.
	///
	/// The minimum height of the area.
	/// @return The minimum height.
	double minimum_height( void ) const { return _minh; }

	bool fits( double w, double h ) const { return w >= _minw && h >= _minh; }

	double x1( void ) const { return _rect.x1(); }
	double y1( void ) const { return _rect.y1(); }
	double x2( void ) const { return _rect.x2(); }
	double y2( void ) const { return _rect.y2(); }

	double x( void ) const { return _rect.x(); }
	double y( void ) const { return _rect.y(); }
	double width( void ) const { return _rect.width(); }
	double height( void ) const { return _rect.height(); }

	draw::point top_left( void ) const { return _rect.top_left(); }
	draw::point top_right( void ) const { return _rect.top_right(); }
	draw::point bottom_left( void ) const { return _rect.bottom_left(); }
	draw::point bottom_right( void ) const { return _rect.bottom_right(); }

	void set_horizontal( double x1, double x2 ) { _rect.set_horizontal( x1, x2 ); }
	void set_vertical( double y1, double y2 ) { _rect.set_vertical( y1, y2 ); }

	void set_size( double w, double h ) { _rect.set_size( w, h ); }

	bool contains( double x, double y ) const { return _rect.contains( x, y ); }
	bool contains( const draw::point &p ) const { return _rect.contains( p.x(), p.y() ); }

	// @brief Compute this area's minimum size
	virtual void compute_minimum( void );

	/// @brief Compute your sub-areas
	virtual void compute_layout( void );

private:
	draw::rect _rect;
	double _minw = 0.0, _minh = 0.0;
};

////////////////////////////////////////

}

