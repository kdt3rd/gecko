
#pragma once

#include "rect.h"

namespace draw
{

////////////////////////////////////////

class area
{
public:
	area( void )
	{
	}

	area( const rect &r )
		: _rect( r )
	{
	}

	area( const point &p, double ww, double hh )
		: _rect( p, ww, hh )
	{
	}

	const rect &rectangle( void ) const { return _rect; }

	void set_minimum_width( double w ) { _minw = w; }
	void set_minimum_height( double h ) { _minh = h; }

	void set_minimum( double w, double h ) { _minw = w; _minh = h; }

	double minimum_width( void ) const { return _minw; }
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

	point top_left( void ) const { return _rect.top_left(); }
	point top_right( void ) const { return _rect.top_right(); }
	point bottom_left( void ) const { return _rect.bottom_left(); }
	point bottom_right( void ) const { return _rect.bottom_right(); }

	void set_horizontal( double x1, double x2 ) { _rect.set_horizontal( x1, x2 ); }
	void set_vertical( double y1, double y2 ) { _rect.set_vertical( y1, y2 ); }

	bool contains( double x, double y ) const { return _rect.contains( x, y ); }

private:
	rect _rect;
	double _minw = 0.0, _minh = 0.0;
};

////////////////////////////////////////

}

