
#pragma once

////////////////////////////////////////

class size
{
public:
	size( void )
	{
	}

	size( double ww, double hh )
		: _w( ww ), _h( hh )
	{
	}

	double width( void ) const { return _w; }
	double height( void ) const { return _h; }

	void set( double ww, double hh )
	{
		_w = ww;
		_h = hh;
	}

	void set_width( double ww ) { _w = ww; }
	void set_height( double hh ) { _h = hh; }

private:
	double _w = 0.0, _h = 0.0;
};

////////////////////////////////////////

