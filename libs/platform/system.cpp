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

#include "system.h"
#include <base/contract.h>

namespace platform
{

////////////////////////////////////////

system::system( std::string name, std::string desc )
	: _name( std::move( name ) ), _desc( std::move( desc ) )
{
}

////////////////////////////////////////

system::~system( void )
{
}

////////////////////////////////////////

std::shared_ptr<timer>
system::create_timer( void )
{
	return std::make_shared<timer>();
}

////////////////////////////////////////

void
system::create_one_off_timer( double in_seconds, const std::function<void(void)> &e )
{
	std::chrono::duration<double, std::milli> fd( in_seconds * 1000.0 );
	create_one_off_timer( timer::clock::now() + std::chrono::duration_cast<timer::duration>( fd ), e );
}

////////////////////////////////////////

void
system::create_one_off_timer( timer::duration in_seconds, const std::function<void(void)> &e )
{
	create_one_off_timer( timer::clock::now() + in_seconds, e );
}

////////////////////////////////////////

void
system::create_one_off_timer( timer::time_point when, const std::function<void(void)> &e )
{
	precondition( e, "Invalid function for one off timer" );
	auto t = create_timer();
	t->elapsed = std::bind( &system::dispatch_oneoff_timer, this, t, e );
	t->reset_one_off( when );
	t->activate( true );
	get_dispatcher()->add_waitable( t );
}

////////////////////////////////////////

void
system::dispatch_oneoff_timer( const std::shared_ptr<timer> &t, const std::function<void(void)> &e )
{
	e();
	get_dispatcher()->remove_waitable( t );
}

}

