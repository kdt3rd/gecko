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
#include "meta.h"
#include "function_traits.h"

////////////////////////////////////////

namespace base
{

/// @defgroup The group of callable routines make a std::function object.
/// @{
template <typename R, typename... Args, int... I>
inline std::function<R (Args...)>
callable( R (*p)( Args... ), int_sequence<I...> )
{
	return std::bind( p, placeholder_template<I>{}...);
}

template <class C, typename R, typename... Args, int... I>
inline std::function<R (Args...)>
callable( R (C::*p)( Args... ), C *o, int_sequence<I...> )
{
	return std::bind( p, o, placeholder_template<I>{}...);
}

template <class C, typename R, typename... Args, int... I>
inline std::function<R (Args...)>
callable( R (C::*p)( Args... ), C &o, int_sequence<I...> )
{
	return std::bind( p, &o, placeholder_template<I>{}...);
}

template <class C, typename R, typename... Args, int... I>
inline std::function<R (Args...)>
callable( R (C::*p)( Args... ) const, const C *o, int_sequence<I...> )
{
	return std::bind( p, o, placeholder_template<I>{}...);
}

template <class C, typename R, typename... Args, int... I>
inline std::function<R (Args...)>
callable( R (C::*p)( Args... ) const, const C &o, int_sequence<I...> )
{
	return std::bind( p, &o, placeholder_template<I>{}...);
}

template <typename R, typename... Args, typename... Extractors>
inline std::function<R (Args...)>
callable( R (*p)(Args...), Extractors &&... )
{
	static_assert(sizeof...(Extractors) == sizeof...(Args), "missing argument binders" );
	return callable( p, make_int_sequence<sizeof...(Args)>{} );
}

template <typename R, typename... Args, typename... Extractors>
inline std::function<R (Args...)>
callable( std::function<R (Args...)> &&f, Extractors &&... )
{
	return std::move( f );
}

template <typename R, typename... Args, typename... Extractors>
inline std::function<R (Args...)>
callable( const std::function<R (Args...)> &f, Extractors &&... )
{
	return f;
}

template <typename Functor, int... I, typename... Extractors>
inline typename function_traits<Functor>::function
callable( Functor f, int_sequence<I...>, Extractors &&... e )
{
	return std::bind( f, placeholder_template<I>{}... );
}

template <typename Functor, typename... Extractors>
inline typename function_traits<Functor>::function
callable( Functor f, Extractors &&... e )
{
	return callable( f, make_int_sequence<sizeof...(Extractors)>{}, std::forward<Extractors>( e )... );
}

/// @}
// end of callable defgroup

// private templated functions used for dispatching a function
// call from a tuple definition of it's arguments
namespace priv {

template <typename R, typename... FuncArgs, typename TupleType, size_t ...S>
inline R
dispatch_help( const std::function<R (FuncArgs...)> &f, const TupleType &args, const sequence<S...> & )
{
	return f( std::get<S>(args)... );
}

template <typename R, typename... FuncArgs, typename TupleType, size_t ...S>
inline R
dispatch_help( R (*f)(FuncArgs...), const TupleType &args, const sequence<S...> & )
{
	return f( std::get<S>(args)... );
}

template <typename... FuncArgs, typename TupleType, size_t ...S>
inline void
dispatch_help( const std::function<void (FuncArgs...)> &f, const TupleType &args, const sequence<S...> & )
{
	f( std::get<S>(args)... );
}

template <typename... FuncArgs, typename TupleType, size_t ...S>
inline void
dispatch_help( void (*f)(FuncArgs...), const TupleType &args, const sequence<S...> & )
{
	f( std::get<S>(args)... );
}

}

/// @defgroup Enables one to store arguments for a function call as a tuple
/// for later execution, then call that function, dereferencing the tuple
/// into the individual arguments
/// @{
template <typename R, typename... FuncArgs, typename TupleType>
inline R
call_func_from_tuple( const std::function<R (FuncArgs...)> &f, const TupleType &args )
{
	static_assert( sizeof...(FuncArgs) == std::tuple_size<TupleType>::value, "Mis-match of function argument count to arguments provided" );
	return priv::dispatch_help( f, args, gen_sequence<sizeof...(FuncArgs)>{} );
}

template <typename... FuncArgs, typename TupleType>
inline void
call_func_from_tuple( const std::function<void (FuncArgs...)> &f, const TupleType &args )
{
	static_assert( sizeof...(FuncArgs) == std::tuple_size<TupleType>::value, "Mis-match of function argument count to arguments provided" );
	priv::dispatch_help( f, args, gen_sequence<sizeof...(FuncArgs)>{} );
}

template <typename R, typename... FuncArgs, typename TupleType>
inline R
call_func_from_tuple( R (*f)(FuncArgs...), const TupleType &args )
{
	static_assert( sizeof...(FuncArgs) == std::tuple_size<TupleType>::value, "Mis-match of function argument count to arguments provided" );
	return priv::dispatch_help( f, args, gen_sequence<sizeof...(FuncArgs)>{} );
}

template <typename... FuncArgs, typename TupleType>
inline void
call_func_from_tuple( void (*f)(FuncArgs...), const TupleType &args )
{
	static_assert( sizeof...(FuncArgs) == std::tuple_size<TupleType>::value, "Mis-match of function argument count to arguments provided" );
	priv::dispatch_help( f, args, gen_sequence<sizeof...(FuncArgs)>{} );
}
/// @}

} // namespace base