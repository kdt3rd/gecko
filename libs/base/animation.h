// Copyright (c) 2017 Ian Godin
// SPDX-License-Identifier: MIT

namespace base
{

////////////////////////////////////////

template<typename Value>
Value ease( const Value &a, const Value &b, double time, double duration )
{
	time *= 2.0/duration;
	if ( time < 1.0 )
		return a + ( b - a ) * ( 0.5 * time * time * time );
	time -= 2.0;
	return a + ( b - a ) * 0.5 * ( time * time * time + 2.0 );
}

////////////////////////////////////////

}

