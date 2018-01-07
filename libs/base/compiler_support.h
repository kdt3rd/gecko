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

#if __cplusplus >= 201703L
# define FALLTHROUGH [[fallthrough]]
#endif

#if __cplusplus >= 201402L
# define PROPER_CONSTEXPR constexpr
#else
# define PROPER_CONSTEXPR
#endif

#if defined(__clang__)
# ifndef FALLTHROUGH
#  define FALLTHROUGH [[clang::fallthrough]];
# endif
# define EMIT_PRAGMA(x) _Pragma(#x)
# define TODO(x) EMIT_PRAGMA(message ("TODO: " x " at line " DEFER(STRINGIZE,__LINE__)))
//# define TODO(x) _Pragma(STRINGIZE(GCC warning x " at line " DEFER(STRINGIZE,__LINE__)))
#else
# if defined(__GNUC__)
#  ifndef FALLTHROUGH
#   define FALLTHROUGH __attribute__((fallthrough))
#  endif
#  if (__GNUC__ < 5)
#   define HAS_MISSING_STREAM_MOVE_CTORS
#   define HAS_BAD_CODECVT_HEADER
#  endif
//#  define TODO(x) _Pragma(STRINGIZE(GCC warning x " at line " DEFER(STRINGIZE,__LINE__)))
#  define EMIT_PRAGMA(x) _Pragma(#x)
#  define TODO(x) EMIT_PRAGMA(message ("TODO: " x " at line " DEFER(STRINGIZE,__LINE__)))
# else
#  if defined(_MSC_VER)
#   define TODO(x) __pragma(STRINGIZE(message(x " at line " DEFER(STRINGIZE,__LINE__))))
#  endif
# endif
#endif

#ifndef FALLTHROUGH
# define FALLTHROUGH
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
