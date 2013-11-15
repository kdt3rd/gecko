
#include "cocoa_style.h"
#include <core/contract.h>
#include "application.h"

namespace gui
{

////////////////////////////////////////

cocoa_style::cocoa_style( void )
{
	_default_font = application::current()->get_font( "Lucida Grande", "Regular", 14.0 );
	postcondition( bool(_default_font), "font for cocoa style not found" );
}

////////////////////////////////////////

cocoa_style::~cocoa_style( void )
{
}

////////////////////////////////////////

std::shared_ptr<draw::font> cocoa_style::default_font( void )
{
	return _default_font;
}

////////////////////////////////////////

}

