//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "compiler_support.h"

////////////////////////////////////////

#if defined(__GNUC__)
# define LIKELY_USE_BUILTIN
#elif defined(__clang__)
# if __has_builtin(__builtin_expect) )
#  define LIKELY_USE_BUILTIN
# endif
#endif

#if defined(LIKELY_USE_BUILTIN)

# ifndef likely
#  define likely(condition) __builtin_expect( condition, 1 )
# endif
# ifndef unlikely
#  define unlikely(condition) __builtin_expect( condition, 0 )
# endif
# undef LIKELY_USE_BUILTIN

#else

# ifndef likely
#  define likely(condition) (condition)
# endif
# ifndef unlikely
#  define unlikely(condition) (condition)
# endif

#endif

