//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "dispatcher.h"
#include <base/contract.h>

namespace platform
{

////////////////////////////////////////

dispatcher::dispatcher( void )
{
}

////////////////////////////////////////

dispatcher::~dispatcher( void )
{
}

////////////////////////////////////////

void
dispatcher::add_waitable( const std::shared_ptr<waitable> &w )
{
	precondition( w, "Invalid waitable to add" );
	for ( auto &cw: _waitables )
	{
		// re-registering an error? not for now
		if ( cw == w )
			return;
	}

	_waitables.push_back( w );
}

////////////////////////////////////////

void
dispatcher::remove_waitable( const std::shared_ptr<waitable> &w )
{
	for ( auto i = _waitables.begin(); i != _waitables.end(); ++i )
	{
		if ( (*i) == w )
		{
			_waitables.erase( i );
			break;
		}
	}
}

////////////////////////////////////////

}

