//
// Copyright (c) 2016 Kimball Thurston
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

#include "compiler_abi.h"
#include "contract.h"
#include "scope_guard.h"
#if defined(__GNUC__)
# include <cxxabi.h>
#endif

////////////////////////////////////////

namespace base
{

std::string
demangle( const char *sym )
{
	precondition( sym && sym[0] != '\0', "invalid symbol passed to demangle" );
	std::string retval;
#if defined(__GNUC__)
	int status = 0;
	char *ptr = abi::__cxa_demangle( sym, nullptr, nullptr, &status );
	on_scope_exit{ if ( ptr ) ::free( ptr ); };

	// per the comment in cxxabi.h, return is 0, -1, -2, -3
	switch ( status )
	{
		case -3: throw_runtime( "invalid argument demangling {0}", sym );
		case -2: throw_runtime( "invalid symbol name demangling {0}", sym );
		case -1: throw_runtime( "memory allocation demangling {0}", sym );
		case 0:
		default:
			retval = ptr;
			break;
	}
#else
#endif
	return retval;
}

} // base



