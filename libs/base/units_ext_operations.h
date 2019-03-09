// Copyright (c) 2018 Kimball Thurston
// SPDX-License-Identifier: MIT

#ifndef GK_BASE_UNITS_H
# error "Do not include units_ext_operations.h by itself, meant as a helper for units.h"
#endif
#include <cstdlib>
#include <base/compiler_support.h>

////////////////////////////////////////

namespace base
{
namespace units
{

////////////////////////////////////////

/// extrinsic operator for differing types
namespace detail
{

template <typename a_length, typename b_length>
struct unit_math
{
    using a_value = typename a_length::value_type;
    using b_value = typename b_length::value_type;
    using convr = typename std::ratio_divide<typename a_length::ratio_to_meters,
                                             typename b_length::ratio_to_meters>;
    using comt = std::common_type_t<a_value, b_value, std::intmax_t>;

    using result_type = length<comt, typename a_length::ratio_to_meters>;

    template <typename Operator>
    static constexpr inline result_type apply( const a_length &a, const b_length &b, Operator f )
    {
        return result_type( std::forward<Operator>(f)( convert<result_type>( a ).count(),
                                                       convert<result_type>( b ).count() ) );
    }
};

} // namespace detail

template <typename tA, typename rA>
constexpr inline length<tA, rA> operator-( const length<tA, rA> &a )
{
    static_assert( ! std::is_unsigned<tA>::value, "unary negate should receive a signed value type" );
    return length<tA, rA>( -a.count() );
}

template <typename tA, typename rA, typename tB, typename rB>
constexpr inline
typename detail::unit_math<length<tA, rA>, length<tB, rB>>::result_type
operator+( const length<tA, rA> &a, const length<tB, rB> &b )
{
    using mather = detail::unit_math<length<tA, rA>, length<tB, rB>>;
    return mather::apply( a, b, []( auto x, auto y ) { return x + y; } );
}

template <typename tA, typename rA, typename tB, typename rB>
constexpr inline
typename detail::unit_math<length<tA, rA>, length<tB, rB>>::result_type
operator-( const length<tA, rA> &a, const length<tB, rB> &b )
{
    using mather = detail::unit_math<length<tA, rA>, length<tB, rB>>;
    return mather::apply( a, b, []( auto x, auto y ) { return x - y; } );
}

template <typename tA, typename rA, typename tB, typename rB>
constexpr inline
typename detail::unit_math<length<tA, rA>, length<tB, rB>>::result_type
operator*( const length<tA, rA> &a, const length<tB, rB> &b )
{
    using mather = detail::unit_math<length<tA, rA>, length<tB, rB>>;
    return mather::apply( a, b, []( auto x, auto y ) { return x * y; } );
}

template <typename OT, typename tB, typename rB,
typename = std::enable_if_t<std::is_convertible<typename std::decay<OT>::type,
                                                typename length<tB, rB>::value_type>::value &&
                            (std::is_floating_point<tB>::value ||
                             (!std::is_floating_point<OT>::value) )>>
constexpr inline
typename detail::unit_math<length<OT,rB>, length<tB, rB>>::result_type
operator*( OT &&a, const length<tB, rB> &b )
{
    return length<OT, rB>( std::forward<OT>( a ) ) * b;
}

template <typename OT, typename tB, typename rB,
typename = std::enable_if_t<std::is_convertible<typename std::decay<OT>::type,
                                                typename length<tB, rB>::value_type>::value &&
                            (std::is_floating_point<tB>::value ||
                             (!std::is_floating_point<OT>::value) )>>
constexpr inline
typename detail::unit_math<length<OT,rB>, length<tB, rB>>::result_type
operator*( const length<tB, rB> &a, OT &&b )
{
    return a * length<OT, rB>( std::forward<OT>( b ) );
}

template <typename tA, typename rA, typename tB, typename rB>
constexpr inline
typename detail::unit_math<length<tA, rA>, length<tB, rB>>::result_type
operator/( const length<tA, rA> &a, const length<tB, rB> &b )
{
    using mather = detail::unit_math<length<tA, rA>, length<tB, rB>>;
    return mather::apply( a, b, []( auto x, auto y ) { return x / y; } );
}

template <typename OT, typename tB, typename rB,
typename = std::enable_if_t<std::is_convertible<typename std::decay<OT>::type,
                                                typename length<tB, rB>::value_type>::value &&
                            (std::is_floating_point<tB>::value ||
                             (!std::is_floating_point<OT>::value) )>>
constexpr inline
typename detail::unit_math<length<OT,rB>, length<tB, rB>>::result_type
operator/( const length<tB, rB> &a, OT &&b )
{
    return a / length<OT, rB>( std::forward<OT>( b ) );
}

////////////////////////////////////////

namespace detail
{

template <typename convr, typename comt, typename CompareFunc, bool numisone = false, bool denisone = false>
struct compare_impl
{
    template <typename aT, typename aRtoM, typename bT, typename bRtoM>
    static constexpr inline bool apply( const length<aT, aRtoM> &a, const length<bT, bRtoM> &b, CompareFunc f )
    {
        return std::forward<CompareFunc>( f )( static_cast<comt>( a.count() ), static_cast<comt>( b.count() ) * static_cast<comt>( convr::num ) / static_cast<comt>( convr::den ) );
    }
};

template <typename convr, typename comt, typename CompareFunc>
struct compare_impl<convr, comt, CompareFunc, true, true>
{
    template <typename aT, typename aRtoM, typename bT, typename bRtoM>
    static constexpr inline bool apply( const length<aT, aRtoM> &a, const length<bT, bRtoM> &b, CompareFunc f )
    {
        return std::forward<CompareFunc>( f )( static_cast<comt>( a.count() ), static_cast<comt>( b.count() ) );
    }
};

template <typename convr, typename comt, typename CompareFunc>
struct compare_impl<convr, comt, CompareFunc, true, false>
{
    template <typename aT, typename aRtoM, typename bT, typename bRtoM>
    static constexpr inline bool apply( const length<aT, aRtoM> &a, const length<bT, bRtoM> &b, CompareFunc f )
    {
        return std::forward<CompareFunc>( f )( static_cast<comt>( a.count() ), static_cast<comt>( b.count() ) / static_cast<comt>( convr::den ) );
    }
};

template <typename convr, typename comt, typename CompareFunc>
struct compare_impl<convr, comt, CompareFunc, false, true>
{
    template <typename aT, typename aRtoM, typename bT, typename bRtoM>
    static constexpr inline bool apply( const length<aT, aRtoM> &a, const length<bT, bRtoM> &b, CompareFunc f )
    {
        return std::forward<CompareFunc>( f )( static_cast<comt>( a.count() ), static_cast<comt>( b.count() ) * static_cast<comt>( convr::num ) );
    }
};

template <typename a_length, typename b_length, typename CompareFunc>
constexpr inline bool compare( const a_length &a, const b_length &b, CompareFunc f )
{
    using a_value = typename a_length::value_type;
    using b_value = typename b_length::value_type;
    using convr = typename std::ratio_divide<typename a_length::ratio_to_meters,
                                             typename b_length::ratio_to_meters>;
    using comt = std::common_type_t<a_value, b_value, std::intmax_t>;
    using comparator = compare_impl<convr, comt, CompareFunc, convr::num == 1, convr::den == 1>;
    return comparator::apply( a, b, std::forward<CompareFunc>( f ) );
}

} // namespace detail

GK_IGNORE_WARNING_BEGIN
GK_IGNORE_WARNINGS
template <typename tA, typename rA, typename tB, typename rB>
inline bool operator==( const length<tA, rA> &a, const length<tB, rB> &b )
{
    return detail::compare( a, b, []( auto x, auto y ) { return x == y; } );
}

template <typename tA, typename rA, typename tB, typename rB>
inline bool operator!=( const length<tA, rA> &a, const length<tB, rB> &b )
{
    return detail::compare( a, b, []( auto x, auto y ) { return x != y; } );
}
GK_IGNORE_WARNING_END

template <typename tA, typename rA, typename tB, typename rB>
inline bool operator<( const length<tA, rA> &a, const length<tB, rB> &b )
{ return detail::compare( a, b, []( auto x, auto y ) { return x < y; } ); }

template <typename tA, typename rA, typename tB, typename rB>
inline bool operator<=( const length<tA, rA> &a, const length<tB, rB> &b )
{ return detail::compare( a, b, []( auto x, auto y ) { return x <= y; } ); }

template <typename tA, typename rA, typename tB, typename rB>
inline bool operator>( const length<tA, rA> &a, const length<tB, rB> &b )
{ return detail::compare( a, b, []( auto x, auto y ) { return x > y; } ); }

template <typename tA, typename rA, typename tB, typename rB>
inline bool operator>=( const length<tA, rA> &a, const length<tB, rB> &b )
{ return detail::compare( a, b, []( auto x, auto y ) { return x >= y; } ); }

////////////////////////////////////////

/// @brief Output operator for length
template <typename tL, typename rL>
inline std::ostream &operator<<( std::ostream &out, const length<tL, rL> &l )
{
	out << l.count();
	return out;
}

////////////////////////////////////////

template <typename tA, typename rA>
inline std::enable_if_t<std::is_floating_point<tA>::value, length<tA, rA>>
round( const length<tA, rA> &a )
{
    return length<tA, rA>( std::round( a.count() ) );
}

template <typename tA, typename rA>
inline std::enable_if_t<std::is_integral<tA>::value, length<tA, rA>>
round( const length<tA, rA> &a )
{
    return a;
}

template <typename tA, typename rA>
inline std::enable_if_t<! std::is_floating_point<tA>::value &&
                        ! std::is_integral<tA>::value, length<tA, rA>>
round( const length<tA, rA> &a )
{
    return length<tA, rA>( std::round( a.count() ) );
}

template <typename tA, typename rA>
inline length<long, rA> lround( const length<tA, rA> &a )
{
    return length<long, rA>( std::lround( a.count() ) );
}

template <typename tA, typename rA>
inline length<long long, rA> llround( const length<tA, rA> &a )
{
    return length<long long, rA>( std::llround( a.count() ) );
}

////////////////////////////////////////

template <typename tA, typename rA>
inline length<tA, rA> abs( const length<tA, rA> &a )
{
    return length<tA, rA>( std::abs( a.count() ) );
}

////////////////////////////////////////

template <typename tA, typename rA>
inline length<tA, rA> distance_squared( const length<tA, rA> &a, const length<tA, rA> &b )
{
    static_assert( std::is_signed<tA>::value, "expect signed type value for length computing distance" );
    tA d = a.count() - b.count();
    return length<tA, rA>( d * d );
}

////////////////////////////////////////

template <typename tA, typename rA>
inline length<tA, rA> distance( const length<tA, rA> &a, const length<tA, rA> &b )
{
    static_assert( std::is_floating_point<tA>::value, "expect floating point value for computing distance" );
    tA d = a.count() - b.count();
    return length<tA, rA>( std::sqrt( d * d ) );
}

////////////////////////////////////////

template <typename tA, typename rA>
inline length<tA, rA> sqrt( const length<tA, rA> &a )
{
    return length<tA, rA>( static_cast<tA>( std::sqrt( a.count() ) ) );
}

////////////////////////////////////////

template <typename tA, typename rA>
inline length<tA, rA> ceil( const length<tA, rA> &a )
{
    return length<tA, rA>( static_cast<tA>( std::ceil( a.count() ) ) );
}

////////////////////////////////////////

template <typename tA, typename rA>
inline length<tA, rA> floor( const length<tA, rA> &a )
{
    return length<tA, rA>( static_cast<tA>( std::floor( a.count() ) ) );
}

////////////////////////////////////////

template <typename tA, typename rA>
inline length<tA, rA> max( const length<tA, rA> &a, const length<tA, rA> &b )
{
    return length<tA, rA>( std::max( a.count(), b.count() ) );
}

////////////////////////////////////////

template <typename tA, typename rA>
inline length<tA, rA> min( const length<tA, rA> &a, const length<tA, rA> &b )
{
    return length<tA, rA>( std::min( a.count(), b.count() ) );
}

} // namespace units
} // namespace base

namespace std
{

template <typename T, typename R>
inline base::units::length<T, R> round( const base::units::length<T, R> &a )
{
	return base::units::round( a );
}

template <typename T, typename R>
inline base::units::length<long, R> lround( const base::units::length<T, R> &a )
{
	return base::units::lround( a );
}

template <typename T, typename R>
inline base::units::length<long long, R> llround( const base::units::length<T, R> &a )
{
	return base::units::llround( a );
}

template <typename T, typename R>
inline base::units::length<T, R> sqrt( const base::units::length<T, R> &a )
{
	return base::units::sqrt( a );
}

template <typename T, typename R>
inline base::units::length<T, R> abs( const base::units::length<T, R> &a )
{
    return base::units::abs( a );
}

template <typename T, typename R>
inline base::units::length<T, R> ceil( const base::units::length<T, R> &a )
{
	return base::units::ceil( a );
}

template <typename T, typename R>
inline base::units::length<T, R> floor( const base::units::length<T, R> &a )
{
	return base::units::floor( a );
}

} // namespace std
