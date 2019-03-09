// Copyright (c) 2018 Kimball Thurston
// SPDX-License-Identifier: MIT

#ifndef GK_BASE_UNITS_H
# error "Do not include units_literals.h by itself, meant as a helper for units.h"
#endif

#pragma once

////////////////////////////////////////

namespace base
{

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
