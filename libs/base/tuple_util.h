//
// Copyright (c) 2014-2015 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <type_traits>
#include <tuple>
#include <utility>
#include "meta.h"

namespace base
{

////////////////////////////////////////

namespace detail
{

template<typename Function, typename Tuple, size_t ...S>
auto expand_call( Function &&f, Tuple &&params, sequence<S...> ) -> decltype( f( std::get<S>( std::forward<Tuple>( params ) )... ) )
{
	return std::forward<Function>( f )( std::get<S>( std::forward<Tuple>( params ) )... );
}

}

////////////////////////////////////////

template<typename Function, typename ...Args>
auto apply( Function &f, const std::tuple<Args...> &params ) -> decltype( detail::expand_call( f, params, typename base::gen_sequence<sizeof...(Args)>::type() ) )
{
	return detail::expand_call( f, params, typename base::gen_sequence<sizeof...(Args)>::type() );
}

////////////////////////////////////////

template<typename T, size_t ...N>
std::tuple<typename std::tuple_element<N, T>::type...> select_tuple( const T &t, sequence<N...> )
{
	return std::make_tuple( std::get<N>(t)... );
}

////////////////////////////////////////

template<typename T, typename R = typename range<1, std::tuple_size<T>::value>::type>
auto tuple_tail( const T &t ) -> decltype( select_tuple( t, R() ) )
{
	return select_tuple( t, R() );
}

////////////////////////////////////////

template <std::size_t I = 0, typename Func, typename... Tp>
inline typename std::enable_if< I == sizeof...(Tp), void >::type
for_each( std::tuple<Tp...> &, Func && )
{
}
template <std::size_t I = 0, typename Func, typename... Tp>
inline typename std::enable_if< I < sizeof...(Tp), void >::type
for_each( std::tuple<Tp...> &t, Func &&f )
{
	std::forward<Func>( f )( std::get<I>( t ) );
	for_each<I + 1, Func, Tp...>( t, std::forward<Func>( f ) );
}

////////////////////////////////////////

template <std::size_t I = 0, typename Func, typename... Tp>
inline typename std::enable_if< I == sizeof...(Tp), void >::type
for_each( const std::tuple<Tp...> &, Func && )
{
}
template <std::size_t I = 0, typename Func, typename... Tp>
inline typename std::enable_if< I < sizeof...(Tp), void >::type
for_each( const std::tuple<Tp...> &t, Func &&f )
{
	std::forward<Func>( f )( std::get<I>( t ) );
	for_each<I + 1, Func, Tp...>( t, std::forward<Func>( f ) );
}

////////////////////////////////////////

}
