// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

////////////////////////////////////////

#define STRINGIZE( x ) #x
#define DEFER( FUNC, ... ) FUNC( __VA_ARGS__ )

// TODO: there are more attributes to handle within here, unclear what
// we need at this point

#if __cplusplus >= 201402L
#    define PROPER_CONSTEXPR constexpr
#    define GK_DEPRECATE( repl )                                               \
        [[deprecated( "use " STRINGIZE( repl ) " instead" )]]
#    if __cplusplus >= 201703L
#        ifndef GK_FALLTHROUGH
#            define GK_FALLTHROUGH [[fallthrough]]
#        endif
#        ifndef GK_UNUSED
#            define GK_UNUSED [[maybe_unused]]
#        endif
#        ifndef GK_CHECK_RETURN
#            define GK_CHECK_RETURN [[nodiscard]]
#        endif
#    endif
#else
#    define PROPER_CONSTEXPR
#endif

#if defined( __clang__ )

#    ifndef GK_FALLTHROUGH
#        define GK_FALLTHROUGH [[clang::fallthrough]]
#    endif
#    ifndef GK_DEPRECATE
#        define GK_DEPRECATE( replmsg ) __attribute__((deprecated("is deprecated", STRINGIZE(repl)))
#    endif

#    define GK_EMIT_PRAGMA( x ) _Pragma( #    x )
#    define TODO( x ) GK_EMIT_PRAGMA( message( " TODO - " x ) )
//# define TODO(x) _Pragma(STRINGIZE(GCC warning x " at line " DEFER(STRINGIZE,__LINE__)))

#    define GK_IGNORE_WARNING_BEGIN GK_EMIT_PRAGMA( clang diagnostic push )
#    define GK_IGNORE_WARNING_END GK_EMIT_PRAGMA( clang diagnostic pop )
#    define GK_IGNORE_WARNINGS                                                 \
        GK_EMIT_PRAGMA( clang diagnostic ignored "-Weverything" )
#elif defined( _MSC_VER )

#    ifndef GK_DEPRECATE
#        define GK_DEPRECATE( replmsg ) __declspec( deprecated )
#    endif
#    define TODO( x )                                                          \
        __pragma( STRINGIZE(                                                   \
            message( x " at line " DEFER( STRINGIZE, __LINE__ ) ) ) )

#elif defined( __GNUC__ ) || defined( __INTEL_COMPILER )

#    ifndef GK_FALLTHROUGH
#        define GK_FALLTHROUGH __attribute__( ( fallthrough ) )
#    endif
#    ifndef GK_UNUSED
#        define GK_UNUSED __attribute__( ( unused ) )
#    endif
#    ifndef GK_DEPRECATE
#        define GK_DEPRECATE( replmsg )                                        \
            __attribute__( ( deprecated( STRINGIZE( replmsg ) ) ) )
#    endif
#    if ( __GNUC__ < 5 )
#        define HAS_MISSING_STREAM_MOVE_CTORS
#        define HAS_BAD_CODECVT_HEADER
#    endif
//#  define TODO(x) _Pragma(STRINGIZE(GCC warning x " at line " DEFER(STRINGIZE,__LINE__)))
#    define GK_EMIT_PRAGMA( x ) _Pragma( #    x )
#    define TODO( x )                                                          \
        GK_EMIT_PRAGMA(                                                        \
            message( "TODO: " x " at line " DEFER( STRINGIZE, __LINE__ ) ) )

#    define GK_IGNORE_WARNING_BEGIN GK_EMIT_PRAGMA( GCC diagnostic push )
#    define GK_IGNORE_WARNING_END GK_EMIT_PRAGMA( GCC diagnostic pop )
#    define GK_IGNORE_WARNINGS GK_EMIT_PRAGMA( GCC diagnostic ignored "-Wall" )

#else
#    error "compiler support not yet filled in for current compiler"
#endif

#ifndef GK_FALLTHROUGH
#    define GK_FALLTHROUGH
#endif
#ifndef GK_UNUSED
#    define GK_UNUSED
#endif
#ifndef GK_DEPRECATE
#    define GK_DEPRECATE( replmsg )
#endif
#ifndef GK_CHECK_RETURN
#    define GK_CHECK_RETURN
#endif

#ifndef __has_builtin
#    define __has_builtin( x ) 0
#endif

#if defined( __GNUC__ ) || defined( __clang__ ) || defined( __INTEL_COMPILER )
#    define GK_FORCE_INLINE inline __attribute__( ( always_inline ) )
#elif defined( _MSVC )
#    define GK_FORCE_INLINE __forceinline
#else
#    define GK_FORCE_INLINE inline
#endif
