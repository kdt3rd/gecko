// Copyright (c) 2018 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once
#ifndef GK_BASE_UNITS_H
# define GK_BASE_UNITS_H 1

#include <cmath>
#include <cstdlib>
#include <ratio>
#include <iostream>

////////////////////////////////////////

namespace base
{

/// @brief Measure of units
///
/// This is modeled after std::chrono where there are compile-time
/// ratios in place for conversion. Unlike std::chrono, math operators
/// are defined.
///
/// Provides type that can be used for units of measure (length
/// primarily, but could be extended in the future)
///
/// NB: includes files at end to put operations in other files to clean up
/// this header
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
	static_assert( std::is_arithmetic<T>::value, "length should be composed of an arithmetic value type" );
    using value_type = T;
    using ratio_to_meters = RtoM;

    constexpr length( void ) = default;

    template <typename OT, typename = std::enable_if_t<std::is_convertible<typename std::decay<OT>::type, value_type>::value &&
                                                       (std::is_floating_point<value_type>::value ||
                                                        (!std::is_floating_point<OT>::value) )>>
    constexpr inline length( OT &&o ) : _val( static_cast<value_type>( o ) ) {}

    constexpr length( const value_type &o ) : _val( o ) {}
    constexpr length( value_type &&o ) : _val( std::move( o ) ) {}

    template <typename OT, typename other_ratio,
              typename = std::enable_if_t<std::is_floating_point<value_type>::value ||
                                          ( std::integral_constant<bool, 1 == std::ratio_divide<other_ratio, ratio_to_meters>::den>::value &&
                                            ( ! std::is_floating_point<OT>::value ) )> >
    constexpr inline length( const length<OT, other_ratio> &o ) : _val( convert<length>( o ).count() ) {}
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

    /// @brief Enable explicit (programmer specified) cast conversion
    template <typename tO, typename rO>
    explicit inline operator length<tO, rO>( void ) const { return length<tO, rO>( *this ); }

    /// @brief alternate (explicit) method of getting @sa count
    explicit constexpr inline operator value_type( void ) const { return _val; }

    /// @brief Retrieve the current count of length units.
    ///
    /// Much like std::chrono, we do not provide implicit conversion to avoid
    /// the loss of units.
    constexpr inline value_type count( void ) const { return _val; }

    inline length &operator=( value_type v ) { _val = v; return *this; }

    inline length &operator++( void ) { ++_val; return *this; }
    inline length operator++( int ) { length tmp(*this); ++_val; return tmp; }

    inline length &operator+=( const length &l ) { _val += l.count(); return *this; }
    inline length &operator+=( value_type v ) { _val += v; return *this; }
    inline length &operator-=( const length &l ) { _val -= l.count(); return *this; }
    inline length &operator-=( value_type v ) { _val -= v; return *this; }
    inline length &operator*=( const length &l ) { _val *= l.count(); return *this; }
    inline length &operator*=( value_type v ) { _val *= v; return *this; }
    inline length &operator/=( const length &l ) { _val /= l.count(); return *this; }
    inline length &operator/=( value_type v ) { _val /= v; return *this; }

    inline length operator+( const length &l ) const { return length( count() + l.count() ); }
    inline length operator+( value_type v ) const { return length( count() + v ); }
    inline length operator-( const length &l ) const { return length( count() - l.count() ); }
    inline length operator-( value_type v ) const { return length( count() - v ); }
    inline length operator*( const length &l ) const { return length( count() * l.count() ); }
    inline length operator*( value_type v ) const { return length( count() * v ); }
    inline length operator/( const length &l ) const { return length( count() / l.count() ); }
    inline length operator/( value_type v ) const { return length( count() / v ); }

private:
    value_type _val;
};

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

/// parsec 3.08567758149137×10^16 - is this exact?
/// per iau 2012
template <typename T> using astronomical_units = length<T, std::ratio<149597870700, 1>>;

////////////////////////////////////////
// imperial units

// 1/1000 of an inch
template <typename T> using mils    = length<T, std::ratio<254,1000*10000>>;
template <typename T> using thous   = mils<T>;
// 1/12 of an inch
template <typename T> using line    = length<T, std::ratio<254,12*10000>>;
template <typename T> using inches  = length<T, std::ratio<254,10000>>;
template <typename T> using feet    = length<T, std::ratio<254*12,10000>>;
template <typename T> using yards   = length<T, std::ratio<254*12*3,10000>>;
template <typename T> using miles   = length<T, std::ratio<254*12*5280,10000>>;
template <typename T> using leagues = length<T, std::ratio<254*12*5280*3,10000>>;


////////////////////////////////////////
// print units

// 6 pica in an inch
template <typename T> using picas   = length<T, std::ratio<254,6*10000>>;
// 1 pica = 12 points
template <typename T> using points   = length<T, std::ratio<254,12*6*10000>>;
// 240 twips to a pica
template <typename T> using twips   = length<T, std::ratio<254,240*6*10000>>;

////////////////////////////////////////
// marine units

template <typename T> using fathoms   = length<T, std::ratio<254*12*3*2,10000>>;
template <typename T> using nautical_miles   = length<T, std::ratio<1852,1>>;

////////////////////////////////////////
// astronomical units

/// 1 A.U. (approx distance earth to sun)
template <typename T> using astronomical_units   = length<T, std::ratio<149597870700,1>>;

} // namespace units

} // namespace base

#include "units_ext_operations.h"
#include "units_literals.h"

#endif // GK_BASE_UNITS_H
