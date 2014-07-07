
#include "font_manager.h"
#include "fontconfig/font_manager.h"

namespace script
{

////////////////////////////////////////

void font_manager::init( void )
{
	enroll( std::make_shared<script::fontconfig::font_manager>() );
}

////////////////////////////////////////

}

