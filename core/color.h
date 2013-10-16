
#pragma once

////////////////////////////////////////

class color
{
public:
	color( void )
	{
	}

	color( double r, double g, double b, double a = 1.0 )
		: _r( r ), _g( g ), _b( b ), _a( a )
	{
	}

	double red( void ) const { return _r; }
	double green( void ) const { return _g; }
	double blue( void ) const { return _b; }
	double alpha( void ) const { return _a; }

	void set( double r, double g, double b, double a = 1.0 )
	{
		_r = r;
		_g = g;
		_b = b;
		_a = a;
	}

private:
	double _r = 0.0, _g = 0.0, _b = 0.0, _a = 0.0;
};

////////////////////////////////////////

