
#include "paint.h"

namespace base
{

////////////////////////////////////////

paint::paint( void )
{
}

////////////////////////////////////////

paint::paint( const color &c, double w )
	: _stroke_color( c ), _stroke_width( w )
{
}

////////////////////////////////////////

paint::~paint( void )
{
}

////////////////////////////////////////

void paint::clear_fill( void )
{
	switch ( _fill_type )
	{
		case NONE:
		case COLOR:
			break;

		case LINEAR:
			_fill_linear.~linear();
			break;

		case RADIAL:
			_fill_radial.~radial();
			break;
	}
	_fill_type = NONE;
}

////////////////////////////////////////

}

