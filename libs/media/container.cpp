
#include "container.h"

namespace media
{

////////////////////////////////////////

container::container( const std::function<void(container&)> &upd )
	: _update( upd )
{
}

////////////////////////////////////////

void container::update( void )
{
	if ( _outofdate )
		_update( *this );
	_outofdate = false;
}

////////////////////////////////////////

}

