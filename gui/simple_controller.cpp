
#include "simple_controller.h"

////////////////////////////////////////

bool simple_controller::process_event( event &e, bool focused )
{
	if ( focused )
	{
		if ( _focus )
			return _focus->process_event( e, focused );
	}
	else
	{
		for ( auto a: _locations )
		{
			if ( a.first->contains( e.x(), e.y() ) )
			{
				if ( a.second->process_event( e, focused ) )
					return true;
			}
		}
	}

	return false;
}

////////////////////////////////////////

