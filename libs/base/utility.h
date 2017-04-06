//
// Copyright (c) 2015 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <utility>


////////////////////////////////////////


namespace base
{

#if __cplusplus <= 201103L
template <typename T, typename U = T>
inline T
exchange( T &obj, U &&nval )
{
	T ret = std::move( obj );
	obj = std::forward<U>( nval );
	return ret;
}
#else
using std::exchange;
#endif

} // namespace base



