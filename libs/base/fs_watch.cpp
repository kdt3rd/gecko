//
// Copyright (c) 2015 Kimball Thurston
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

#include "fs_watch.h"
#include "utility.h"


////////////////////////////////////////


namespace base
{


////////////////////////////////////////


fs_watcher::fs_watcher( void )
{
}


////////////////////////////////////////


fs_watcher::~fs_watcher( void )
{
}


////////////////////////////////////////


fs_watch::fs_watch( const std::shared_ptr<fs_watcher> &fsw,
					const event_handler &evt )
		: _fsw( fsw ), _evt_cb( evt )
{
}


////////////////////////////////////////


fs_watch::~fs_watch( void )
{
	if ( _fsw )
		_fsw->unregisterWatch( *this );
}


////////////////////////////////////////


fs_watch::fs_watch( fs_watch &&o )
		: _fsw( exchange( o._fsw, std::shared_ptr<fs_watcher>() ) ), _evt_cb( std::move( o._evt_cb ) )
{
	_fsw->move( o, *this );
}


////////////////////////////////////////


fs_watch &
fs_watch::operator=( fs_watch &&o )
{
	_fsw = exchange( o._fsw, std::shared_ptr<fs_watcher>() );
	_evt_cb = std::move( o._evt_cb );
	_fsw->move( o, *this );
	return *this;
}


////////////////////////////////////////


} // base



