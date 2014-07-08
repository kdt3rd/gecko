
#include "font_manager.h"
#include "cocoa/font_manager.h"

namespace script
{

////////////////////////////////////////

void font_manager::init( void )
{
	enroll( std::make_shared<script::cocoa::font_manager>() );
}

////////////////////////////////////////

}

