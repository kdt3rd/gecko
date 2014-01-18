
#pragma once

#include <string>
#include <vector>

#include "gradient.h"
#include <core/color.h>
#include <core/point.h>
#include <core/contract.h>

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
	paint( void );
	paint( const core::color &c );
	~paint( void );
	
	bool has_antialias( void ) const { return _antialias; }
	void set_antialias( bool on = true ) { _antialias = on; }

	void set_stroke( const core::color &c, double w ) { _stroke_color = c; _stroke_width = w; }

	void set_stroke_color( const core::color &c ) { _stroke_color = c; }
	const core::color &get_stroke_color( void ) const { return _stroke_color; }

	void set_stroke_width( double w ) { _stroke_width = w; }
	double get_stroke_width( void ) const { return _stroke_width; }

	void clear_fill( void );
	bool has_no_fill( void ) const { return _fill_type == NONE; }

	void set_fill_color( const core::color &c ) { clear_fill(); _fill_type = COLOR; _fill_color = c; }
	bool has_fill_color( void ) const { return _fill_type == COLOR; }
	const core::color &get_fill_color( void ) const { precondition( has_fill_color(), "no fill color" ); return _fill_color; }

	void set_fill_linear( const core::point &p1, const core::point &p2, const gradient &g ) { clear_fill(); _fill_type = LINEAR; new (&_fill_linear) linear( p1, p2, g ); }
	void set_fill_linear( const core::point &p1, double w, double h, const gradient &g ) { set_fill_linear( p1, { p1.x() + w, p1.y() + h }, g ); }
	bool has_fill_linear( void ) const { return _fill_type == LINEAR; }
	const core::point &get_fill_linear_p1( void ) const { precondition( has_fill_linear(), "no fill linear" ); return _fill_linear.p1; }
	const core::point &get_fill_linear_p2( void ) const { precondition( has_fill_linear(), "no fill linear" ); return _fill_linear.p2; }
	const std::vector<std::pair<double,core::color>> &get_fill_linear_stops( void ) const { precondition( has_fill_linear(), "no fill linear" ); return _fill_linear.grad.stops(); }

	void set_fill_radial( const core::point &p1, double r1, const core::point &p2, double r2, const gradient &g ) { clear_fill(); _fill_type = RADIAL; new (&_fill_radial) radial( p1, r1, p2, r2, g ); }
	void set_fill_radial( const core::point &p, double r, const gradient &g ) { set_fill_radial( p, 0.0, p, r, g ); }
	bool has_fill_radial( void ) const { return _fill_type == RADIAL; }
	const core::point &get_fill_radial_p1( void ) const { precondition( has_fill_radial(), "no fill radial" ); return _fill_radial.p1; }
	const core::point &get_fill_radial_p2( void ) const { precondition( has_fill_radial(), "no fill radial" ); return _fill_radial.p2; }
	double get_fill_radial_r1( void ) const { precondition( has_fill_radial(), "no fill radial" ); return _fill_radial.r1; }
	double get_fill_radial_r2( void ) const { precondition( has_fill_radial(), "no fill radial" ); return _fill_radial.r2; }
	const std::vector<std::pair<double,core::color>> &get_fill_radial_stops( void ) const { precondition( has_fill_radial(), "no fill radial" ); return _fill_radial.grad.stops(); }

private:
	bool _antialias = true;

	core::color _stroke_color;
	double _stroke_width = 1.0;

	enum
	{
		NONE,
		COLOR,
		LINEAR,
		RADIAL,
	} _fill_type = NONE;

	struct linear
	{
		linear( const core::point &pp1, const core::point &pp2, const gradient &g )
			: p1( pp1 ), p2( pp2 ), grad( g )
		{
		}

		core::point p1, p2;
		gradient grad;
	};

	struct radial
	{
		radial( const core::point &pp1, double rr1, const core::point &pp2, double rr2, const gradient &g )
			: p1( pp1 ), p2( pp2 ), r1( rr1 ), r2( rr2 ), grad( g )
		{
		}

		core::point p1, p2;
		double r1, r2;
		gradient grad;
	};

	union
	{
		core::color _fill_color;
		linear _fill_linear;
		radial _fill_radial;
	};
};

////////////////////////////////////////

}

