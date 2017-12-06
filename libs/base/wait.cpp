//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "wait.h"
#ifndef _WIN32
# include <sys/select.h>
#endif
#include "contract.h"

////////////////////////////////////////

namespace base
{

void wait_for( const wait &w )
{
#ifdef _WIN32
	DWORD r = WaitForSingleObject( w.waitable(), 0 );
	if ( r == WAIT_FAILED )
		throw_lasterror( "Unable to wait on object" );
#else
	// TODO: signal handling / pselect???
	fd_set waitreadobjs;
	FD_ZERO( &waitreadobjs );
	int nWaits = w.waitable();
	FD_SET( nWaits, &waitreadobjs );
	int selrv = select( nWaits, &waitreadobjs, NULL, NULL, NULL );
	if ( selrv < 0 )
		throw_errno( "Error waiting for events to be available" );
#endif
}

wait wait_for_any( const std::vector<wait> &w )
{
#ifdef _WIN32
	DWORD ncount = static_cast<DWORD>( w.size() );
	// cheater
	precondition( sizeof(wait) == sizeof(HANDLE), "invalid assumption about HANDLE / LONG_PTR equivalence" );
	const HANDLE *h = reinterpret_cast<const HANDLE *>( w.data() );
	DWORD r = WaitForMultipleObjects( ncount, h, false, 0 );
	if ( r == WAIT_FAILED )
		throw_lasterror( "Unable to wait on objects" );
	DWORD last = WAIT_OBJECT_0 + ncount;
	if ( r >= WAIT_OBJECT_0 && r < last )
		return w[r - WAIT_OBJECT_0];

	throw_lasterror( "Other error waiting for objects" );
#else
	// TODO: signal handling / pselect???
	fd_set waitreadobjs;
	FD_ZERO( &waitreadobjs );
	int nWaits = -1;
	for ( auto &a: w )
	{
		int afd = a.waitable();
		nWaits = std::max( nWaits, afd );
		FD_SET( afd, &waitreadobjs );
	}
	int selrv = select( nWaits, &waitreadobjs, NULL, NULL, NULL );
	if ( selrv < 0 )
		throw_errno( "Error waiting for events to be available" );

	for ( auto &a: w )
	{
		if ( FD_ISSET( a.waitable(), &waitreadobjs ) )
			return a;
	}
	throw_logic( "Unable to find file descriptor waited upon" );
#endif
}

////////////////////////////////////////

} // base



