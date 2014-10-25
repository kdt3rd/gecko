
#pragma once

#include <type_traits>
#include <tuple>
#include "meta.h"

namespace base
{

////////////////////////////////////////

namespace tuple_impl
{

template<typename Function, typename Tuple, size_t ...S>
auto expand_call( Function &f, Tuple &&params, sequence<S...> ) -> decltype( f( std::get<S>( std::forward<Tuple>( params ) )... ) )
{
	return f( std::get<S>( std::forward<Tuple>( params ) )... );
}

}

////////////////////////////////////////

template<typename Function, typename ...Args>
auto apply( Function &f, const std::tuple<Args...> &params ) -> decltype( tuple_impl::expand_call( f, params, typename base::gen_sequence<sizeof...(Args)>::type() ) )
{
	return tuple_impl::expand_call( f, params, typename base::gen_sequence<sizeof...(Args)>::type() );
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

}
