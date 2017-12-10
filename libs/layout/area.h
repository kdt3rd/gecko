//
// Copyright (c) 2017 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <base/size.h>
#include <base/rect.h>
#include <limits>

namespace layout
{

////////////////////////////////////////

/// @brief Rectangular area for laying out
///
/// Represents an area, with a minimum size.
/// Used by layouts and contains basic layout methods.
class area : public base::rect<double>
{
public:
	using base_type = base::rect<double>;
	using coord_type = base_type::coord_type;
	using point_type = base_type::point_type;
	using size_type = base_type::size_type;

	using base_type::rect;

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

	/// @brief The minimum size of the area.
	///
	/// @return The minimum size.
	size_type minimum_size( void ) const { return _min; }

	/// @brief Minimum width.
	///
	/// The minimum width of the area.
	/// @return The minimum width.
	coord_type minimum_width( void ) const { return _min.w(); }

	/// @brief Minimum height.
	///
	/// The minimum height of the area.
	/// @return The minimum height.
	coord_type minimum_height( void ) const { return _min.h(); }

	/// @brief Set minimum size.
	///
	/// @param s The minimum size to set.
	void set_minimum( const size_type &s ){ _min = s; }

	/// @brief Set minimum size.
	///
	/// @param w The minimum width.
	/// @param h The minimum height.
	void set_minimum( coord_type w, coord_type h ) { _min.set( w, h ); }

	/// @brief Set minimum width.
	///
	/// @param w The minimum width.
	void set_minimum_width( coord_type w ) { _min.set_width( w ); }

	/// @brief Set minimum height.
	///
	/// @param h The minimum height.
	void set_minimum_height( coord_type h ) { _min.set_height( h ); }

	/// @brief The maximum size of the area.
	///
	/// @return The maximum size.
	size_type maximum_size( void ) const { return _max; }

	/// @brief Maximum width.
	///
	/// The maximum width of the area.
	/// @return The maximum width.
	coord_type maximum_width( void ) const { return _max.w(); }

	/// @brief Maximum height.
	///
	/// The maximum height of the area.
	/// @return The maximum height.
	coord_type maximum_height( void ) const { return _max.h(); }

	/// @brief Set maximum size.
	///
	/// @param s The maximum size to set.
	void set_maximum( const size_type &s ){ _max = s; }

	/// @brief Set maximum size.
	///
	/// @param w The maximum width.
	/// @param h The maximum height.
	void set_maximum( coord_type w, coord_type h ) { _max.set( w, h ); }

	/// @brief Set maximum width.
	///
	/// @param w The maximum width.
	void set_maximum_width( coord_type w ) { _max.set_width( w ); }

	/// @brief Set maximum height.
	///
	/// @param h The maximum height.
	void set_maximum_height( coord_type h ) { _max.set_height( h ); }

	/// @brief Priority for expanding areas.
	///
	/// When multiple areas can be expanded, the higher priority ones are expanded first.
	int32_t expansion_priority( void ) const { return _priority; }

	/// @brief Set the priority for expanding areas.
	///
	void set_expansion_priority( int32_t p ) { _priority = p; }

	void set_expansion_flex( coord_type f = 1.0 );

	coord_type expansion_flex( void ) const { return _flex; }

	/// @brief Expand the width of the area.
	///
	/// The width is expanded by at most w.
	/// The area will not expand beyond it's maximum size.
	/// @param w The maximum amount to expand by.
	/// @return The amount of the space used by the expansion.
	coord_type expand_width( coord_type w, coord_type overall_flex );

	/// @brief Expand the height of the area.
	///
	/// The height is expanded by at most h.
	/// Any left over size is returned.
	/// The area will not expand beyond it's maximum size.
	/// @param h The maximum amount to expand by.
	/// @return The amount of the space used by the expansion.
	coord_type expand_height( coord_type h, coord_type overall_flex );

	/// @brief Compute the maximum and minimum size of this area.
	///
	/// By default, this does nothing and the current minimum and maximum are kept as is.
	/// However, classes derived from area can override this and compute their own bounds on the fly.
	/// This method should normally be called before computing a layout.
	virtual void compute_bounds( void );

	/// @brief Compute the position and size of children areas
	///
	/// By default, this does nothing because there are no children.
	/// Classes deriving from area can override this and layout out their child areas as they wish.
	virtual void compute_layout( void );

private:
	size_type _min = { 0.0, 0.0 };
	size_type _max = { 1e6, 1e6 };
	coord_type _flex = 0.0;
	int32_t _priority = 0;
};

////////////////////////////////////////

}

