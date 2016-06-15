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

////////////////////////////////////////

# define STRINGIZE(x) #x
# define DEFER(FUNC,...) FUNC(__VA_ARGS__)

#if defined(__clang__)
# define FALLTHROUGH [[clang::fallthrough]];
# define TODO(x) _Pragma(STRINGIZE(GCC warning x " at line " DEFER(STRINGIZE,__LINE__)))
#else
# if defined(__GNUC__)
#  define FALLTHROUGH [[fallthrough]]
#  if (__GNUC__ < 5)
#   define HAS_MISSING_STREAM_MOVE_CTORS
#   define HAS_BAD_CODECVT_HEADER
#  endif
#  define TODO(x) _Pragma(STRINGIZE(GCC warning x " at line " DEFER(STRINGIZE,__LINE__)))
# else
#  if defined(_MSC_VER)
#   define TODO(x) __pragma(STRINGIZE(message(x " at line " DEFER(STRINGIZE,__LINE__))))
#  endif
#  define FALLTHROUGH
# endif
#endif

