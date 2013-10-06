
#pragma once

#include "event.h"

////////////////////////////////////////

class controller
{
public:
	virtual ~controller( void )
	{
	}

	virtual bool process_event( event &ev, bool focused ) = 0;
};

////////////////////////////////////////

