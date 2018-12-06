//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT
//

#include "system.h"

#include <base/contract.h>

#include "dispatcher.h"
#include "timer.h"


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

