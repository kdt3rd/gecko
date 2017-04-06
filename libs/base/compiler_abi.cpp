//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
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



