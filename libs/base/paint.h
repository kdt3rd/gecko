
#pragma once

#include <string>
#include <vector>

#include "gradient.h"
#include "color.h"
#include "point.h"
#include "size.h"
#include "contract.h"

namespace base
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
	paint( void );
	paint( const color &c, double w = 1.0 );
	~paint( void );
	
	void set_stroke( const color &c, double w ) { _stroke_color = c; _stroke_width = w; }

	void set_stroke_color( const color &c ) { _stroke_color = c; }
	const color &get_stroke_color( void ) const { return _stroke_color; }

	void set_stroke_width( double w ) { _stroke_width = w; }
	double get_stroke_width( void ) const { return _stroke_width; }

	void clear_fill( void );
	bool has_no_fill( void ) const { return _fill_type == NONE; }

	bool has_fill_color( void ) const { return _fill_type == COLOR; }
	void set_fill_color( const color &c ) { clear_fill(); _fill_type = COLOR; _fill_color = c; }
	const color &get_fill_color( void ) const { precondition( has_fill_color(), "no fill color" ); return _fill_color; }

	bool has_fill_linear( void ) const { return _fill_type == LINEAR; }
	void set_fill_linear( const point &p, const size &s, const gradient &g ) { clear_fill(); _fill_type = LINEAR; new (&_fill_linear) linear( p, s, g ); }
	void set_fill_linear( const point &p1, double w, double h, const gradient &g ) { set_fill_linear( p1, { w, h }, g ); }
	const point &get_fill_linear_origin( void ) const { precondition( has_fill_linear(), "no fill linear" ); return _fill_linear.p; }
	const size &get_fill_linear_size( void ) const { precondition( has_fill_linear(), "no fill linear" ); return _fill_linear.s; }
	const gradient &get_fill_linear_gradient( void ) const { precondition( has_fill_linear(), "no fill linear" ); return _fill_linear.grad; }

	bool has_fill_radial( void ) const { return _fill_type == RADIAL; }
	void set_fill_radial( const point &p1, double r1, const point &p2, double r2, const gradient &g ) { clear_fill(); _fill_type = RADIAL; new (&_fill_radial) radial( p1, r1, p2, r2, g ); }
	void set_fill_radial( const point &p, double r, const gradient &g ) { set_fill_radial( p, 0.0, p, r, g ); }
	const point &get_fill_radial_p1( void ) const { precondition( has_fill_radial(), "no fill radial" ); return _fill_radial.p1; }
	const point &get_fill_radial_p2( void ) const { precondition( has_fill_radial(), "no fill radial" ); return _fill_radial.p2; }
	double get_fill_radial_r1( void ) const { precondition( has_fill_radial(), "no fill radial" ); return _fill_radial.r1; }
	double get_fill_radial_r2( void ) const { precondition( has_fill_radial(), "no fill radial" ); return _fill_radial.r2; }
	const gradient &get_fill_radial_gradient( void ) const { precondition( has_fill_radial(), "no fill radial" ); return _fill_radial.grad; }

	bool has_fill_conical( void ) const { return _fill_type == CONICAL; }
	void set_fill_conical( const point &p, const gradient &g ) { clear_fill(); _fill_type = CONICAL; new (&_fill_conical) conical( p, g ); }
	const point &get_fill_conical_center( void ) const { precondition( has_fill_conical(), "no fill conical" ); return _fill_conical.p; }
	const gradient &get_fill_conical_gradient( void ) const { precondition( has_fill_conical(), "no fill conical" ); return _fill_conical.grad; }

private:
	color _stroke_color;
	double _stroke_width = 0.0;

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
		radial( const point &pp1, double rr1, const point &pp2, double rr2, const gradient &g )
			: p1( pp1 ), p2( pp2 ), r1( rr1 ), r2( rr2 ), grad( g )
		{
		}

		point p1, p2;
		double r1, r2;
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

	union
	{
		color _fill_color;
		linear _fill_linear;
		radial _fill_radial;
		conical _fill_conical;
	};
};

////////////////////////////////////////

}

