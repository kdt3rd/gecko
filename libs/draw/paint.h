//
// Copyright (c) 2014-2016 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <string>
#include <vector>

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
	paint( const gl::color &c, float w = 1.F );

	/// @brief Destructor.
	~paint( void );

	/// @brief Set the stroke color and width.
	/// @param c Color of stroke.
	/// @param w Width of stroke.
	void set_stroke( const gl::color &c, float w ) { _stroke_color = c; _stroke_width = w; }

	/// @brief Set the stroke color.
	/// @param c Color of stroke.
	void set_stroke_color( const gl::color &c ) { _stroke_color = c; }

	/// @brief Get the stroke color.
	const gl::color &get_stroke_color( void ) const { return _stroke_color; }

	/// @brief Set the stroke width.
	/// @param w Width of stroke.
	void set_stroke_width( float w ) { _stroke_width = w; }

	/// @brief Get the stroke width.
	float get_stroke_width( void ) const { return _stroke_width; }

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
	void set_fill_color( const gl::color &c ) { clear_fill(); _fill_type = COLOR; _fill_color = c; }

	/// @brief Return true if solid color fill.
	bool has_fill_color( void ) const { return _fill_type == COLOR; }

	/// @brief Get the fill color.
	const gl::color &get_fill_color( void ) const { precondition( has_fill_color(), "no fill color" ); return _fill_color; }

	/// @}

	/// @name Linear gradient fill.
	/// Functions relating to having a linear gradient fill.
	/// @{
	bool has_fill_linear( void ) const { return _fill_type == LINEAR; }
	void set_fill_linear( const gl::vec2 &p, const gl::vec2 &s, const gradient &g ) { clear_fill(); _fill_type = LINEAR; new (&_fill_linear) linear( p, s, g ); }
	void set_fill_linear( const gl::vec2 &p1, float w, float h, const gradient &g ) { set_fill_linear( p1, { w, h }, g ); }

	const gl::vec2 &get_fill_linear_origin( void ) const { precondition( has_fill_linear(), "no fill linear" ); return _fill_linear.p; }
	const gl::vec2 &get_fill_linear_size( void ) const { precondition( has_fill_linear(), "no fill linear" ); return _fill_linear.s; }
	const gradient &get_fill_linear_gradient( void ) const { precondition( has_fill_linear(), "no fill linear" ); return _fill_linear.grad; }

	/// @}

	/// @name Radial gradient fill.
	/// Functions relating to having a radial gradient fill.
	/// @{

	bool has_fill_radial( void ) const { return _fill_type == RADIAL; }
	void set_fill_radial( const gl::vec2 &p1, float r1, const gl::vec2 &p2, float r2, const gradient &g ) { clear_fill(); _fill_type = RADIAL; new (&_fill_radial) radial( p1, r1, p2, r2, g ); }
	void set_fill_radial( const gl::vec2 &p, float r, const gradient &g ) { set_fill_radial( p, 0.0, p, r, g ); }
	const gl::vec2 &get_fill_radial_p1( void ) const { precondition( has_fill_radial(), "no fill radial" ); return _fill_radial.p1; }
	const gl::vec2 &get_fill_radial_p2( void ) const { precondition( has_fill_radial(), "no fill radial" ); return _fill_radial.p2; }
	float get_fill_radial_r1( void ) const { precondition( has_fill_radial(), "no fill radial" ); return _fill_radial.r1; }
	float get_fill_radial_r2( void ) const { precondition( has_fill_radial(), "no fill radial" ); return _fill_radial.r2; }
	const gradient &get_fill_radial_gradient( void ) const { precondition( has_fill_radial(), "no fill radial" ); return _fill_radial.grad; }

	/// @}

	/// @name Conical gradient fill.
	/// Functions relating to having a conical gradient fill.
	/// @{

	bool has_fill_conical( void ) const { return _fill_type == CONICAL; }
	void set_fill_conical( const gl::vec2 &p, const gradient &g ) { clear_fill(); _fill_type = CONICAL; new (&_fill_conical) conical( p, g ); }
	const gl::vec2 &get_fill_conical_center( void ) const { precondition( has_fill_conical(), "no fill conical" ); return _fill_conical.p; }
	const gradient &get_fill_conical_gradient( void ) const { precondition( has_fill_conical(), "no fill conical" ); return _fill_conical.grad; }

	/// @}

	bool empty( void ) const
	{
		return _stroke_width == 0.0 || has_no_fill();
	}

private:
	gl::color _stroke_color;
	float _stroke_width = 0.0;

	enum
	{
		NONE,
		COLOR,
		LINEAR,
		RADIAL,
		CONICAL
	} _fill_type = NONE;

	struct linear
	{
		linear( const gl::vec2 &pt, const gl::vec2 &sz, const gradient &g )
			: p( pt ), s( sz ), grad( g )
		{
		}

		gl::vec2 p;
		gl::vec2 s;
		gradient grad;
	};

	struct radial
	{
		radial( const gl::vec2 &pp1, float rr1, const gl::vec2 &pp2, float rr2, const gradient &g )
			: p1( pp1 ), p2( pp2 ), r1( rr1 ), r2( rr2 ), grad( g )
		{
		}

		gl::vec2 p1, p2;
		float r1, r2;
		gradient grad;
	};

	struct conical
	{
		conical( const gl::vec2 &pp, const gradient &g )
			: p( pp ), grad( g )
		{
		}

		gl::vec2 p;
		gradient grad;
	};

	union
	{
		gl::color _fill_color;
		linear _fill_linear;
		radial _fill_radial;
		conical _fill_conical;
	};
};

////////////////////////////////////////

}

