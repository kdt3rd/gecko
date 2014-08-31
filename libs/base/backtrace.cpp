
#include <stdio.h>
#include <execinfo.h>
#include <stdlib.h>
#include <unistd.h>
#include "backtrace.h"

namespace base
{

void backtrace( void )
{
	void *array[10];
	size_t size;

	size = ::backtrace( array, 10 );
	::backtrace_symbols_fd( array, size, STDERR_FILENO );
}

}

