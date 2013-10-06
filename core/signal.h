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

#include <algorithm>
#include <functional>
#include "slot.h"

using namespace std;

namespace core
{

////////////////////////////////////////

// When a signal is emitted, all slots attached to it are called.
template<typename function>
class signal
{
public:
	typedef typename slot<function>::connection_id connection_id;

	signal( void )
	{
	}

	signal( const signal & ) = delete;

	~signal( void )
	{
		for ( auto s: _slots )
			s.first->disconnect( s.second );
	}

	connection_id connect( slot<function> &s )
	{
		auto cid = s.on_disconnect( bind( &signal<function>::remove_slot, this, &s ) );
		_slots.push_back( make_pair( &s, cid ) );
		return cid;
	}

	template<typename ...args>
	void emit( args... params )
	{
		for ( auto s: _slots )
			s.first->call( params... );
	}

	template<typename ...args>
	void operator()( args... params )
	{
		emit( params... );
	}

	signal<function> operator=( const signal<function> & ) = delete;

private:
	void remove_slot( slot<function> *s )
	{
		auto test = [=]( const pair<slot<function>*,connection_id> &p ) { return p.first == s; };
		_slots.erase( remove_if( _slots.begin(), _slots.end(), test ), _slots.end() );
	}

	vector<pair<slot<function> *,connection_id>> _slots;
};

////////////////////////////////////////

}

// vim:ft=cpp:
