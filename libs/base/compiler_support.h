//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
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

#ifndef __has_builtin
# define __has_builtin(x) 0
#endif

#if defined(__GNUC__) || defined(__clang__)
# define GK_FORCE_INLINE inline __attribute__((always_inline))
#elif defined(_WIN32)
# define GK_FORCE_INLINE __forceinline
#else
# define GK_FORCE_INLINE inline
#endif
