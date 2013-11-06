
#include <iostream>
#include <stdlib.h>
#include <core/contract.h>
#include <core/pointer.h>
#include "dispatcher.h"
#include <windows.h>

namespace mswin
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

	MSG Msg;

	while( GetMessage(&Msg, NULL, 0, 0) > 0 )
	{
		TranslateMessage( &Msg );
		DispatchMessage( &Msg );
	}

	return _exit_code;
}

////////////////////////////////////////

void dispatcher::exit( int code )
{
	_exit_code = code;
}

////////////////////////////////////////

}

