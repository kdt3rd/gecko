
#pragma once

#include <draw/rect.h>

namespace layout
{

////////////////////////////////////////

/// @brief Rectangular area
///
/// Represents an area, with a minimum size.  Used by layouts, views, and reactions.
class simple_area : public draw::rect
{
public:
	using draw::rect::rect;

	virtual ~simple_area( void );

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

	// @brief Compute this area's minimum size
	virtual void compute_minimum( void );

	/// @brief Compute your sub-areas
	virtual void compute_layout( void );

private:
	double _minw = 0.0, _minh = 0.0;
};

////////////////////////////////////////

}

