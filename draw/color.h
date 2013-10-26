
#pragma once

namespace draw
{

////////////////////////////////////////

class color
{
public:
	enum class space
	{
		SRGB,
		LAB
	};

	color( void )
	{
	}

	color( double r, double g, double b, double a = 1.0 )
		: _r( r ), _g( g ), _b( b ), _a( a )
	{
	}

	color( space s, double i, double j, double k, double a = 1.0 )
		: _a( a )
	{
		switch ( s )
		{
			case space::SRGB: set( i, j, k ); break;
			case space::LAB: set_lab( i, j, k ); break;
		}
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

	void get_lab( double &l, double &a, double &b );
	void set_lab( double l, double a, double b );

	color desaturate( double amt );
	color change( double amt );

private:
	double _r = 0.0, _g = 0.0, _b = 0.0, _a = 0.0;
};

////////////////////////////////////////

}

