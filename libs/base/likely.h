//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

////////////////////////////////////////

// bleh, everyone and their uncle defines these, but there's no
// standard. use (hopefully) unique names to avoid define clashes
//
// some patterns use !!(cond), others just use (cond), hard to
// say which is better
#if defined(__clang__)

# if __has_builtin(__builtin_expect)
#  define GK_LIKELY(condition) __builtin_expect( (condition), 1 )
#  define GK_UNLIKELY(condition) __builtin_expect( (condition), 0 )
#  define GK_EXPECT(condition, outcome) __builtin_expect( (condition), (outcome) )
# endif

#elif defined(__GNUC__) && (__GNUC__ >= 3)

# define GK_LIKELY(condition) __builtin_expect( (condition), 1 )
# define GK_UNLIKELY(condition) __builtin_expect( (condition), 0 )
# define GK_EXPECT(condition, outcome) __builtin_expect( (condition), (outcome) )

#elif defined(__INTEL_COMPILER)

# define GK_LIKELY(condition) __builtin_expect( (condition), 1 )
# define GK_UNLIKELY(condition) __builtin_expect( (condition), 0 )
# define GK_EXPECT(condition, outcome) __builtin_expect( (condition), (outcome) )

#endif

#ifndef GK_LIKELY
# define GK_LIKELY(condition) (condition)
#endif
#ifndef GK_UNLIKELY
# define GK_UNLIKELY(condition) (condition)
#endif
#ifndef GK_EXPECT
# define GK_EXPECT(condition, outcome) (condition)
#endif


