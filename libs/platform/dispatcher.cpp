//
// Copyright (c) 2016 Gecko project
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

