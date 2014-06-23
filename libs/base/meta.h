
#pragma once

#include <tuple>

namespace base
{

/// @brief Disable if B is derived from A (or same)
template<typename A, typename B>
using disable_if_same_or_derived = typename std::enable_if<!std::is_base_of<A,typename std::remove_reference<B>::type>::value>::type;

////////////////////////////////////////

/// @brief Convert enumeration to integer
template<typename E>
auto as_integer( E const v ) -> typename std::underlying_type<E>::type
{
	return static_cast<typename std::underlying_type<E>::type>( v );
}

////////////////////////////////////////

/// @brief Sum several numbers together
template<typename H, typename ...T>
H sum( H h, T ...t )
{
	return h + sum<T...>( t... );
}

/// @brief End of recursive sum
template<>
inline size_t sum<size_t>( size_t h )
{
	return h;
}

////////////////////////////////////////

/// @brief Return the nth variadic type
template<size_t N,typename ...Args>
class nth_variadic
{
	typedef typename std::tuple_element<N, std::tuple<Args...>>::type type;
};

////////////////////////////////////////

}

