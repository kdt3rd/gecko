// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

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
callable( Functor f, int_sequence<I...>, Extractors &&... )
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
