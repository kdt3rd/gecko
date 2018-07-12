//
// Copyright (c) 2014-2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <string>
#include <vector>

#include "types.h"
#include "gradient.h"
#include <gl/color.h>
#include <gl/vector.h>
#include <base/contract.h>

namespace draw
{

////////////////////////////////////////

/// @brief Paint to color with.
///
/// Paint describes how to draw paths and text.
/// It is made of 2 parts: stroke and fill.
/// The stroke is the line width, style, and color used to outline the path or text.
/// The fill is used to fill the inside of the path or text.
/// The fill can be a single color, a gradient (linear or radial).
class paint
{
public:
	/// @brief Default constructor.
	/// No stroke and no fill.
	paint( void );

	/// @brief Stroke constructor.
	/// Paint with the given stroke of the given width and no fill.
	/// @param c Color of stroke.
	/// @param w Width of stroke.
	paint( const color &c, dim w = dim(1) );

	/// @brief Destructor.
	~paint( void );

	/// @brief Set the stroke color and width.
	/// @param c Color of stroke.
	/// @param w Width of stroke.
	void set_stroke( const color &c, dim w ) { _stroke_color = c; _stroke_width = w; }

	/// @brief Set the stroke color.
	/// @param c Color of stroke.
	void set_stroke_color( const color &c ) { _stroke_color = c; }

	/// @brief Get the stroke color.
	const color &get_stroke_color( void ) const { return _stroke_color; }

	/// @brief Set the stroke width.
	/// @param w Width of stroke.
	void set_stroke_width( dim w ) { _stroke_width = w; }

	/// @brief Get the stroke width.
	dim get_stroke_width( void ) const { return _stroke_width; }

	/// @name No fill paint.
	/// Functions relating to having no fill paint.
	/// @{

	/// @brief Clear the fill paint.
	void clear_fill( void );

	/// @brief Returns true if no fill.
	bool has_no_fill( void ) const { return _fill_type == NONE; }

	bool has_fill( void ) const { return _fill_type != NONE; }

	/// @}

	/// @name Solid color fill.
	/// Functions relating to having a fill color.
	/// @{

	/// @brief Set the fill color.
	/// Set the fill to a single solid color.
	void set_fill_color( const color &c ) { clear_fill(); _fill_type = COLOR; _fill_color = c; }

	/// @brief Return true if solid color fill.
	bool has_fill_color( void ) const { return _fill_type == COLOR; }

	/// @brief Get the fill color.
	const color &get_fill_color( void ) const { precondition( has_fill_color(), "no fill color" ); return _fill_color; }

	/// @}

	/// @name Linear gradient fill.
	/// Functions relating to having a linear gradient fill.
	/// @{
	bool has_fill_linear( void ) const { return _fill_type == LINEAR; }
	void set_fill_linear( const point &p, const size &s, const gradient &g ) { clear_fill(); _fill_type = LINEAR; new (&_fill_linear) linear( p, s, g ); }
	void set_fill_linear( const point &p, dim w, dim h, const gradient &g ) { set_fill_linear( p, { w, h }, g ); }

	const point &get_fill_linear_origin( void ) const { precondition( has_fill_linear(), "no fill linear" ); return _fill_linear.p; }
	const size &get_fill_linear_size( void ) const { precondition( has_fill_linear(), "no fill linear" ); return _fill_linear.s; }
	const gradient &get_fill_linear_gradient( void ) const { precondition( has_fill_linear(), "no fill linear" ); return _fill_linear.grad; }

	/// @}

	/// @name Radial gradient fill.
	/// Functions relating to having a radial gradient fill.
	/// @{

	bool has_fill_radial( void ) const { return _fill_type == RADIAL; }
	void set_fill_radial( const point &p1, dim r1, dim r2, const gradient &g ) { clear_fill(); _fill_type = RADIAL; new (&_fill_radial) radial( p1, r1, r2, g ); }
	void set_fill_radial( const point &p, dim r, const gradient &g ) { set_fill_radial( p, dim(0), r, g ); }
	const point &get_fill_radial_center( void ) const { precondition( has_fill_radial(), "no fill radial" ); return _fill_radial.center; }
	dim get_fill_radial_r1( void ) const { precondition( has_fill_radial(), "no fill radial" ); return _fill_radial.r1; }
	dim get_fill_radial_r2( void ) const { precondition( has_fill_radial(), "no fill radial" ); return _fill_radial.r2; }
	const gradient &get_fill_radial_gradient( void ) const { precondition( has_fill_radial(), "no fill radial" ); return _fill_radial.grad; }

	/// @}

	/// @name Conical gradient fill.
	/// Functions relating to having a conical gradient fill.
	/// @{

	bool has_fill_conical( void ) const { return _fill_type == CONICAL; }
	void set_fill_conical( const point &p, const gradient &g ) { clear_fill(); _fill_type = CONICAL; new (&_fill_conical) conical( p, g ); }
	const point &get_fill_conical_center( void ) const { precondition( has_fill_conical(), "no fill conical" ); return _fill_conical.p; }
	const gradient &get_fill_conical_gradient( void ) const { precondition( has_fill_conical(), "no fill conical" ); return _fill_conical.grad; }

	/// @}

	/// @name Box gradient fill.
	/// Functions relating to having a box gradient fill.
	/// @{
	bool has_fill_box( void ) const { return _fill_type == BOX; }
	void set_fill_box( const point &p1, const point &p2, dim radius, const gradient &g ) { clear_fill(); _fill_type = BOX; new (&_fill_box) box( p1, p2, radius, g ); }

	const point &get_fill_box_point1( void ) const { precondition( has_fill_box(), "no fill box" ); return _fill_box.point1; }
	const point &get_fill_box_point2( void ) const { precondition( has_fill_box(), "no fill box" ); return _fill_box.point2; }
	dim get_fill_box_radius( void ) const { precondition( has_fill_box(), "no fill box" ); return _fill_box.radius; }
	const gradient &get_fill_box_gradient( void ) const { precondition( has_fill_box(), "no fill box" ); return _fill_box.grad; }

	/// @}

	bool empty( void ) const
	{
		return _stroke_width == dim(0) && has_no_fill();
	}

private:
	color _stroke_color;
	dim _stroke_width = dim(0);

	enum
	{
		NONE,
		COLOR,
		LINEAR,
		RADIAL,
		CONICAL,
		BOX,
	} _fill_type = NONE;

	struct linear
	{
		linear( const point &pt, const size &sz, const gradient &g )
			: p( pt ), s( sz ), grad( g )
		{
		}

		point p;
		size s;
		gradient grad;
	};

	struct radial
	{
		radial( const point &c, dim rr1, dim rr2, const gradient &g )
			: center( c ), r1( rr1 ), r2( rr2 ), grad( g )
		{
		}

		point center;
		dim r1, r2;
		gradient grad;
	};

	struct conical
	{
		conical( const point &pp, const gradient &g )
			: p( pp ), grad( g )
		{
		}

		point p;
		gradient grad;
	};

	struct box
	{
		box( const point &p1, const point &p2, dim r, const gradient &g )
			: point1( p1 ), point2( p2 ), radius( r ), grad( g )
		{
		}

		point point1;
		point point2;
		dim radius;
		gradient grad;
	};

	union
	{
		color _fill_color;
		linear _fill_linear;
		radial _fill_radial;
		conical _fill_conical;
		box _fill_box;
	};
};

////////////////////////////////////////

}
