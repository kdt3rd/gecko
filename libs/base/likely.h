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

