
#pragma once

#include "color.h"

namespace draw
{

////////////////////////////////////////

class paint
{
public:
	paint( void );
	paint( const color &c );
	~paint( void );
	
	void set_color( const color &c ) { _color = c; }
	const color &get_color( void ) const { return _color; }

	bool has_anti_aliasing( void ) const { return _anti_aliasing; }
	void set_anti_aliasing( bool on = true ) { _anti_aliasing = on; }

	void set_stroke_width( double w ) { _stroke_width = w; }
	double get_stroke_width( void ) const { return _stroke_width; }

private:
	color _color;
	bool _anti_aliasing = true;
	double _stroke_width = 1.0;
};

////////////////////////////////////////

}

