
#pragma once

#include <tuple>
#include <functional>

namespace base
{

////////////////////////////////////////

/// @brief Disable if B is derived from A (or same)
template<typename A, typename B>
using disable_if_same_or_derived = typename std::enable_if<!std::is_base_of<A,typename std::remove_reference<B>::type>::value>::type;

////////////////////////////////////////

template<bool ...>
struct bool_sequence
{
};

////////////////////////////////////////

template<bool ...Bs>
using bool_and = std::is_same<bool_sequence<Bs...>, bool_sequence<( Bs || true )...>>;

template<bool ...Bs>
using bool_or = std::integral_constant<bool, !bool_and<!Bs...>::value>;

////////////////////////////////////////

template<typename R, bool ...Bs>
using enable_if_any = std::enable_if<bool_or<Bs...>::value, R>;

template<typename R, bool ...Bs>
using enable_if_all = std::enable_if<bool_and<Bs...>::value, R>;

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
constexpr H sum( H h, T ...t )
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
struct nth_variadic
{
	typedef typename std::tuple_element<N, std::tuple<Args...>>::type type;
};

////////////////////////////////////////

/// @brief Statically compute max template argument.
template <size_t Arg1, size_t ...Others>
struct static_max;

template <size_t Arg>
struct static_max<Arg>
{
	static const size_t value = Arg;
};

template <size_t Arg1, size_t Arg2, size_t ...Others>
struct static_max<Arg1, Arg2, Others...>
{
	static const size_t value = ( Arg1 >= Arg2 ) ? static_max<Arg1, Others...>::value : static_max<Arg2, Others...>::value;
};

////////////////////////////////////////

template<typename ...T>
struct always_false : std::false_type
{
};

////////////////////////////////////////

/// @defgroup used to generate a sequence of indices as a variadic
/// thing so we can deref a tuple or vector of values as a series of
/// arguments
///
/// Should be used something like:
///
/// gen_sequence<Args...>{}
///
/// @{
template<size_t...S>
struct sequence
{
	template<size_t X>
	using cons = sequence<X,S...>;
};

template<size_t N, size_t ...S>
struct gen_sequence : gen_sequence<N-1, N-1, S...>
{
};

template<size_t ...S>
struct gen_sequence<0, S...> : sequence<S...>
{
	typedef sequence<S...> type;
};
/// @}

////////////////////////////////////////

template<size_t Start, size_t End>
struct range
{
	static_assert( Start < End, "invalid range" );
	typedef typename range<Start+1,End>::type::template cons<Start> type;
};

template<size_t Start>
struct range<Start,Start>
{
	typedef sequence<> type;
};

////////////////////////////////////////

/// @defgroup same as sequence, but integer type
/// @{
template <int ...> struct int_sequence {};
template <int N, int... I> struct make_int_sequence
    : make_int_sequence<N-1, N-1, I...> {};
template<int... I> struct make_int_sequence<0, I...>
    : int_sequence<I...>
{
	typedef int_sequence<I...> type;
};
/// @}

/// @brief used for making a variadic bind
template <int> struct placeholder_template {};

} // namespace base

namespace std
{

/// @brief provide partial specialization to indicate our variadic
/// placeholder is a placeholder
template <int N>
struct is_placeholder< base::placeholder_template<N> > : integral_constant<int, N+1>
{};

} // namespace std

