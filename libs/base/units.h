//
// Copyright (c) 2018 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <cmath>
#include <ratio>

////////////////////////////////////////

namespace base
{

/// @brief Modelled after std::chrono
///
/// Provides type that can be used for units of measure (length
/// primarily, but could be extended in the future)
namespace units
{

template <typename T, typename ratio_to_meters = std::ratio<1> >
class length;

template <typename T> struct is_length : std::false_type
{};

template <typename T, typename Rm>
struct is_length< length<T, Rm> > : std::true_type
{};

#if __cplusplus >= 201703L
template <typename T> inline constexpr bool is_length_v = is_length<T>::value;
#endif

// avoid math if we can
namespace detail
{

template <typename out, typename convr, typename comt, bool numisone = false, bool denisone = false>
struct convert_impl
{
    template <typename T, typename RtoM>
    static constexpr inline out apply( const length<T, RtoM> &l )
    {
        using value_type = typename out::value_type;
        return out( static_cast<value_type>( static_cast<comt>( l.count() ) * static_cast<comt>( convr::num ) / static_cast<comt>( convr::den ) ) );
    }
};

template <typename out, typename convr, typename comt>
struct convert_impl<out, convr, comt, true, true>
{
    template <typename T, typename RtoM>
    static constexpr inline out apply( const length<T, RtoM> &l )
    {
        using value_type = typename out::value_type;
        return out( static_cast<value_type>( l.count() ) );
    }
};

template <typename out, typename convr, typename comt>
struct convert_impl<out, convr, comt, true, false>
{
    template <typename T, typename RtoM>
    static constexpr inline out apply( const length<T, RtoM> &l )
    {
        using value_type = typename out::value_type;
        return out( static_cast<value_type>( static_cast<comt>( l.count() ) / static_cast<comt>( convr::den ) ) );
    }
};

template <typename out, typename convr, typename comt>
struct convert_impl<out, convr, comt, false, true>
{
    template <typename T, typename RtoM>
    static constexpr inline out apply( const length<T, RtoM> &l )
    {
        using value_type = typename out::value_type;
        return out( static_cast<value_type>( static_cast<comt>( l.count() ) * static_cast<comt>( convr::num ) ) );
    }
};

} // namespace detail

/// @brief convert one length to another
template <typename out_length, typename in_length>
constexpr inline std::enable_if_t<is_length<out_length>::value && is_length<in_length>::value, out_length>
convert( const in_length &l )
{
    using to_value = typename out_length::value_type;
    using from_value = typename in_length::value_type;
    using convr = typename std::ratio_divide<typename in_length::ratio_to_meters,
                                             typename out_length::ratio_to_meters>;
    using comt = std::common_type_t<to_value, from_value, std::intmax_t>;
    using converter = detail::convert_impl<out_length, convr, comt, convr::num == 1, convr::den == 1>;
    return converter::apply( l );
}

/// @brief storage for a unit of length
///
/// everything is relative to meters, but each sub type can store at
/// whatever precision it requires.
template <typename T, typename RtoM>
class length
{
public:
    using value_type = T;
    using ratio_to_meters = RtoM;

    constexpr length( void ) = default;

    template <typename OT, typename = std::enable_if_t<std::is_convertible<OT, value_type>::value &&
                                                       (std::is_floating_point<value_type>::value ||
                                                        (!std::is_floating_point<OT>::value) )>>
    constexpr explicit length( const OT &o ) : _val( static_cast<value_type>( o ) ) {}

    template <typename OT, typename other_ratio,
              typename = std::enable_if_t<std::is_floating_point<value_type>::value ||
                                          ( std::integral_constant<bool, 1 == std::ratio_divide<other_ratio, ratio_to_meters>::den>::value &&
                                            ( ! std::is_floating_point<OT>::value ) )> >
    constexpr length( const length<OT, other_ratio> &o ) : _val( convert<length>( o ).count() ) {}
    ~length( void ) = default;
    length( const length & ) = default;
    length &operator=( const length & ) = default;
    length( length && ) = default;
    length &operator=( length && ) = default;

    template <typename OT>
    constexpr inline
    std::enable_if_t<
        is_length<OT>::value &&
        ( std::is_floating_point<value_type>::value ||
          ( std::integral_constant<bool, 1 == std::ratio_divide<typename OT::ratio_to_meters, ratio_to_meters>::den>::value &&
            ( ! std::is_floating_point<typename OT::value_type>::value ) ) ), OT>
    as( void ) const
    {
        return convert<OT>( *this );
    }

    constexpr inline value_type count( void ) const { return _val; }

private:
    value_type _val;
};

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

template <typename tA, typename rA, typename tB, typename rB>
inline bool operator==( const length<tA, rA> &a, const length<tB, rB> &b )
{ return detail::compare( a, b, []( auto a, auto b ) { return a == b; } ); }

template <typename tA, typename rA, typename tB, typename rB>
inline bool operator!=( const length<tA, rA> &a, const length<tB, rB> &b )
{ return detail::compare( a, b, []( auto a, auto b ) { return a != b; } ); }

template <typename tA, typename rA, typename tB, typename rB>
inline bool operator<( const length<tA, rA> &a, const length<tB, rB> &b )
{ return detail::compare( a, b, []( auto a, auto b ) { return a < b; } ); }

template <typename tA, typename rA, typename tB, typename rB>
inline bool operator<=( const length<tA, rA> &a, const length<tB, rB> &b )
{ return detail::compare( a, b, []( auto a, auto b ) { return a <= b; } ); }

template <typename tA, typename rA, typename tB, typename rB>
inline bool operator>( const length<tA, rA> &a, const length<tB, rB> &b )
{ return detail::compare( a, b, []( auto a, auto b ) { return a > b; } ); }

template <typename tA, typename rA, typename tB, typename rB>
inline bool operator>=( const length<tA, rA> &a, const length<tB, rB> &b )
{ return detail::compare( a, b, []( auto a, auto b ) { return a >= b; } ); }

////////////////////////////////////////
// standard SI units

template <typename T> using exameters   = length<T, std::exa>;
template <typename T> using petameters  = length<T, std::peta>;
template <typename T> using terameters  = length<T, std::tera>;
template <typename T> using gigameters  = length<T, std::giga>;
template <typename T> using megameters  = length<T, std::mega>;
template <typename T> using kilometers  = length<T, std::kilo>;
template <typename T> using hectometers = length<T, std::hecto>;
template <typename T> using decameters  = length<T, std::deca>;
template <typename T> using meters      = length<T, std::ratio<1,1>>;
template <typename T> using decimeters  = length<T, std::deci>;
template <typename T> using centimeters = length<T, std::centi>;
template <typename T> using millimeters = length<T, std::milli>;
template <typename T> using micrometers = length<T, std::micro>;
template <typename T> using nanometers  = length<T, std::nano>;
template <typename T> using picometers  = length<T, std::pico>;
template <typename T> using femtometers = length<T, std::femto>;
template <typename T> using attometers  = length<T, std::atto>;

////////////////////////////////////////
// non-SI units

template <typename T> using fermi = femtometers<T>;
template <typename T> using micron = micrometers<T>;
/// angstrom = 100 picometers
template <typename T> using angstroms = length<T, std::ratio<1, 10000000000>>;

////////////////////////////////////////
// imperial units

// 1/1000 of an inch
template <typename T> using mils    = length<T, std::ratio<1,1>>;
template <typename T> using thous   = mils<T>;
// 1/12 of an inch
template <typename T> using line    = length<T, std::ratio<1,1>>;
template <typename T> using inches  = length<T, std::ratio<254,10000>>;
template <typename T> using feet    = length<T, std::ratio<254*12,10000>>;
template <typename T> using yards   = length<T, std::ratio<254*12*3,10000>>;
template <typename T> using miles   = length<T, std::ratio<254*12*5280,10000>>;
template <typename T> using leagues = length<T, std::ratio<254*12*5280*3,10000>>;

////////////////////////////////////////
// marine units

template <typename T> using fathoms   = length<T, std::ratio<254*12*3*2,10000>>;
template <typename T> using nautical_miles   = length<T, std::ratio<1852,1>>;

////////////////////////////////////////
// astronomical units

/// 1 A.U. (approx distance earth to sun)
template <typename T> using astronomical_units   = length<T, std::ratio<149597870700,1>>;

} // namespace units

////////////////////////////////////////

inline namespace literals
{

inline namespace length_literals
{

constexpr inline units::meters<long double> operator"" _km( long double x )
{ return units::kilometers<long double>( x ); }

constexpr inline units::meters<unsigned long long int> operator"" _km( unsigned long long int x )
{ return units::kilometers<unsigned long long int>( x ); }

constexpr inline units::meters<long double> operator"" _m( long double x )
{ return units::meters<long double>( x ); }

constexpr inline units::meters<unsigned long long int> operator"" _m( unsigned long long int x )
{ return units::meters<unsigned long long int>( x ); }

constexpr inline units::centimeters<long double> operator"" _cm( long double x )
{ return units::centimeters<long double>( x ); }

constexpr inline units::centimeters<unsigned long long int> operator"" _cm( unsigned long long int x )
{ return units::centimeters<unsigned long long int>( x ); }

constexpr inline units::millimeters<long double> operator"" _mm( long double x )
{ return units::millimeters<long double>( x ); }

constexpr inline units::millimeters<unsigned long long int> operator"" _mm( unsigned long long int x )
{ return units::millimeters<unsigned long long int>( x ); }

constexpr inline units::inches<long double> operator"" _in( long double x )
{ return units::inches<long double>( x ); }

constexpr inline units::inches<unsigned long long int> operator"" _in( unsigned long long int x )
{ return units::inches<unsigned long long int>( x ); }

} // namespace length_literals

} // namespace literals

namespace units
{
using namespace literals::length_literals;
}

} // namespace base



