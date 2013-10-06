
#pragma once

#include <core/signal.h>

////////////////////////////////////////

class area
{
public:
	area( void )
	{
	}

	area( double x1, double y1, double x2, double y2 )
		: _x1( x1 ), _y1( y1 ), _x2( x2 ), _y2( y2 )
	{
	}

	void set_xy( double x, double y );

	void set_minimum_width( double w ) { _minw = w; }
	void set_minimum_height( double h ) { _minh = h; }

	void set_minimum( double w, double h ) { _minw = w; _minh = h; }

	core::signal<void(void)> &minimum_changed( void ) { return _minimum; }
	core::signal<void(void)> &area_changed( void ) { return _area; }

	double minimum_width( void ) const { return _minw; }
	double minimum_height( void ) const { return _minh; }

	double x1( void ) const { return _x1; }
	double y1( void ) const { return _y1; }
	double x2( void ) const { return _x2; }
	double y2( void ) const { return _y2; }

	void set_horizontal( double x1, double x2 ) { _x1 = x1; _x2 = x2; }
	void set_vertical( double y1, double y2 ) { _y1 = y1; _y2 = y2; }

	double width( void ) const { return _x2 - _x1; }
	double height( void ) const { return _y2 - _y1; }

	bool contains( double x, double y ) const;

private:
	double _x1 = 0.0, _y1 = 0.0, _x2 = 0.0, _y2 = 0.0;
	double _minw = 0.0, _minh = 0.0;

	core::signal<void(void)> _minimum;
	core::signal<void(void)> _area;
};

////////////////////////////////////////

