//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <base/size.h>
#include <base/rect.h>

namespace layout
{

////////////////////////////////////////

/// @brief Rectangular area for laying out
///
/// Represents an area, with a minimum size.
/// Used by layouts and contains basic layout methods.
class area : public base::rect
{
public:
	using base::rect::rect;

	/// @brief Default constructor.
	area( void ) = default;

	/// @brief Copy constructor.
	area( const area & ) = default;

	/// @brief Move constructor.
	area( area && ) = default;

	/// @brief Copy assignment.
	area &operator=( const area & ) = default;

	/// @brief Move assignment.
	area &operator=( area && ) = default;

	virtual ~area( void ) { }

	/// @brief Set minimum width.
	///
	/// @param w The minimum width.
	void set_minimum_width( double w ) { _min.set_width( w ); }

	/// @brief Set minimum height.
	///
	/// @param h The minimum height.
	void set_minimum_height( double h ) { _min.set_height( h ); }

	/// @brief Set minimum size.
	///
	/// @param w The minimum width.
	/// @param h The minimum height.
	void set_minimum( double w, double h ) { _min.set( w, h ); }

	/// @brief Set minimum size.
	///
	/// @param s The minimum size to set.
	void set_minimum( const base::size &s ){ _min = s; }

	/// @brief Minimum width.
	///
	/// The minimum width of the area.
	/// @return The minimum width.
	double minimum_width( void ) const { return _min.w(); }

	/// @brief Minimum height.
	///
	/// The minimum height of the area.
	/// @return The minimum height.
	double minimum_height( void ) const { return _min.h(); }

	/// @brief The minimum size of the area.
	///
	/// @return The minimum size.
	base::size minimum( void ) const { return _min; }

	/// @brief Check if the area fits in the give size.
	///
	/// @param w Width to check fit.
	/// @param h Height to check fit.
	/// @return True if the area fits.
	bool fits( double w, double h ) const { return w >= _min.w() && h >= _min.h(); }

	/// @brief Compute the minimum size of this area.
	///
	/// By default, this does nothing.
	virtual void compute_minimum( void );

	/// @brief Compute the position and size of children
	///
	/// By default, this does nothing.
	virtual void compute_layout( void );

private:
	base::size _min;
};

////////////////////////////////////////

}

