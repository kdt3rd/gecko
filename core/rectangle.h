
#pragma once

////////////////////////////////////////

class rectangle
{
public:
	rectangle( void )
	{
	}

	rectangle( double x1, double y1, double x2, double y2 )
		: _x1( x1 ), _y1( y1 ), _x2( x2 ), _y2( y2 )
	{
	}

	void set_xy( double x, double y );

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
};

////////////////////////////////////////

