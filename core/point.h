
#pragma once

////////////////////////////////////////

class point
{
public:
	point( void )
	{
	}

	point( double xx, double yy )
		: _x( xx ), _y( yy )
	{
	}

	double x( void ) const { return _x; }
	double y( void ) const { return _y; }

	void set( double xx, double yy )
	{
		_x = xx;
		_y = yy;
	}

	void set_x( double xx ) { _x = xx; }
	void set_y( double yy ) { _y = yy; }

private:
	double _x = 0.0, _y = 0.0;
};

////////////////////////////////////////

