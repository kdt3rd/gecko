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

#include <cstddef>
#include <functional>
#include <type_traits>
#include "meta.h"
#include <tuple>

////////////////////////////////////////

namespace base
{

template <typename R, typename... Args>
struct base_traits
{
	static constexpr size_t arity = sizeof...(Args);
	typedef R result_type;
	typedef R (*pointer)( Args... );

	template <size_t N>
	using get_arg_type = base::nth_variadic<N, Args...>;

	typedef std::function<R ( Args... )> function;
};

template <typename... Args>
struct base_traits<void, Args...>
{
	static constexpr size_t arity = sizeof...(Args);
	typedef void (*pointer)( Args... );

	template <size_t N>
	using get_arg_type = base::nth_variadic<N, Args...>;

	typedef std::function<void ( Args... )> function;
};

/// @defgroup Define some traits about the provided functor
/// @{

/// the first version is default is a recursive template to extract functor information
template <typename Function>
struct function_traits : public function_traits<decltype(&Function::operator())>
{
};

/// specialization for a function type
template <typename R, typename... Args>
struct function_traits<R ( Args... )> : base_traits<R, Args...>
{
};

/// specialization for a function pointer type
template <typename R, typename... Args>
struct function_traits<R (*)( Args... )> : base_traits<R, Args...>
{
};

/// specialization for a std::function
template <typename R, typename... Args>
struct function_traits< std::function<R ( Args... )>> : base_traits<R, Args...>
{
};
	
/// specialization for a class member function
template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)> : base_traits<R, Args...>
{
};
	
/// specialization for a const class member function
template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) const> : base_traits<R, Args...>
{
};

/// @}

/// @brief simple function binder
///
/// this won't work for pointer to members, so we need to add a binder
/// type object below
template <typename Functor>
inline typename function_traits<Functor>::function
to_function( Functor f )
{
	return static_cast<typename function_traits<Functor>::function>( f );
}

namespace priv {

template <typename C, typename R, typename... Args, int... I>
inline std::function<R (Args...)>
binder_help( R(C::*memF)(Args...), C *o, int_sequence<I...> )
{
	return std::bind( memF, o, placeholder_template<I>{}... );
}

template <typename C, typename R, typename... Args, int... I>
inline std::function<R (Args...)>
binder_help( R(C::*memF)(Args...) const, const C *o, int_sequence<I...> )
{
	return std::bind( memF, o, placeholder_template<I>{}... );
}

}

template <typename C, typename R, typename... Args>
inline std::function<R (Args...)>
to_function( R(C::*memF)(Args...), C *o )
{
	return priv::binder_help( memF, o, make_int_sequence<sizeof...(Args)>{} );
}

template <typename C, typename R, typename... Args>
inline std::function<R (Args...)>
to_function( R(C::*memF)(Args...), C &o )
{
	return priv::binder_help( memF, &o, make_int_sequence<sizeof...(Args)>{} );
}

template <typename C, typename R, typename... Args>
inline std::function<R (Args...)>
to_function( R(C::*memF)(Args...) const, const C *o )
{
	return priv::binder_help( memF, o, make_int_sequence<sizeof...(Args)>{} );
}

template <typename C, typename R, typename... Args>
inline std::function<R (Args...)>
to_function( R(C::*memF)(Args...) const, const C &o )
{
	return priv::binder_help( memF, &o, make_int_sequence<sizeof...(Args)>{} );
}

} // namespace base



