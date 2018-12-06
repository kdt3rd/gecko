//
// Copyright (c) 2015 Ian Godin
// SPDX-License-Identifier: MIT
//

#include <signal.h>

int main( void )
{
	raise( SIGSEGV );
}
