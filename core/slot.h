//
// Copyright (c) 2012 Ian Godin
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

#pragma once

#include <list>
#include <functional>

using namespace std;

namespace core
{

////////////////////////////////////////

// A slot represents a function which can be called.
template<typename func>
class slot
{
public:
	typedef list<function<void(void)>>::iterator connection_id;

	slot( const function<func> &f )
		: _function( f )
	{
	}

	slot( void ) = delete;
	slot( const slot & ) = delete;

	virtual ~slot( void )
	{
		disconnect();
	}

	template<typename ...args>
	void call( args ... params )
	{
		_function( params... );
	}

	void disconnect( void )
	{
		for ( auto callback: _callbacks )
			callback();
		_callbacks.clear();
	}

	void disconnect( const connection_id &id )
	{
		_callbacks.erase( id );
	}

	connection_id on_disconnect( const function<void(void)> &callback )
	{
		return _callbacks.insert( _callbacks.end(), callback );
	}

private:
	function<func> _function;
	list<function<void(void)>> _callbacks;
};

////////////////////////////////////////

}

// vim:ft=cpp:
