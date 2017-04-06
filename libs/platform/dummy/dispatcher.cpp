//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <iostream>
#include <stdlib.h>
#include <base/contract.h>
#include <base/pointer.h>
#include "dispatcher.h"

namespace platform { namespace dummy
{

////////////////////////////////////////

dispatcher::dispatcher( const std::shared_ptr<keyboard> &k, const std::shared_ptr<mouse> &m )
	: _keyboard( k ), _mouse( m )
{
}

////////////////////////////////////////

dispatcher::~dispatcher( void )
{
}

////////////////////////////////////////

int dispatcher::execute( void )
{
	_exit_code = 0;

	bool done = false;
	while ( !done )
	{
//		::sleep( 1 );
	}

	return _exit_code;
}

////////////////////////////////////////

void dispatcher::exit( int code )
{
	_exit_code = code;
}

////////////////////////////////////////

} }

