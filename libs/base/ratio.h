//
// Copyright (c) 2014-2016 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include <cstdint>
#include <iostream>
#include <utility>

namespace base
{

////////////////////////////////////////

class ratio
{
public:
	ratio( void ) = default;
	constexpr ratio( int64_t n, int64_t d ) : _num( n ), _den( d ) {}
	ratio( int64_t n, int64_t d, bool simp );
	ratio( const ratio & ) = default;
	ratio( ratio && ) noexcept = default;
	~ratio( void ) = default;
	ratio &operator=( const ratio & ) = default;
	ratio &operator=( ratio && ) noexcept = default;

	void set( int64_t n, int64_t d );

	inline constexpr bool valid( void ) const;

	inline constexpr int64_t numerator( void ) const;
	inline constexpr int64_t denominator( void ) const;

	inline constexpr double value( void ) const;

	inline constexpr ratio invert( void ) const;
	inline constexpr int64_t trunc( void ) const;
	inline constexpr int64_t mod( void ) const;
	inline constexpr int64_t round( void ) const;

	ratio &operator +=( const ratio &r );
	ratio &operator -=( const ratio &r );
	ratio &operator *=( const ratio &r );
	ratio &operator /=( const ratio &r );
	ratio &operator +=( int64_t n );
	ratio &operator -=( int64_t n );
	ratio &operator *=( int64_t n );
	ratio &operator /=( int64_t n );

	/// @brief Returns this rebased to a common denominator with r
	ratio common( const ratio &r ) const;

private:
	void simplify( void );

	int64_t _num = 0;
	int64_t _den = 0;
};

////////////////////////////////////////

inline constexpr bool ratio::valid( void ) const
{
	return _den != 0;
}

inline constexpr int64_t ratio::numerator( void ) const
{
	return _num;
}

inline constexpr int64_t ratio::denominator( void ) const
{
	return _den;
}

inline constexpr double ratio::value( void ) const
{
	return static_cast<double>(_num) / static_cast<double>(_den);
}

inline constexpr ratio ratio::invert( void ) const
{
	return ratio( _den, _num );
}

inline constexpr int64_t ratio::trunc( void ) const
{
	return _num / _den;
}

inline constexpr int64_t ratio::mod( void ) const
{
	return _num % _den;
}

inline constexpr int64_t ratio::round( void ) const
{
	return ( _num + ( _num < 0 ? -_den : _den ) / 2 ) / _den;
}

////////////////////////////////////////

/// @brief returns 2 ratios with the denominator in common
std::pair<ratio,ratio> rebase( const ratio &a, const ratio &b );

////////////////////////////////////////

/// @defgroup RatioMath ratio related simple math operators
///
/// @{
ratio operator+( const ratio &a, const ratio &b );
ratio operator+( const ratio &a, int64_t b );
ratio operator+( int64_t a, const ratio &b );
ratio operator-( const ratio &a, const ratio &b );
ratio operator-( const ratio &a, int64_t b );
ratio operator-( int64_t a, const ratio &b );
ratio operator*( const ratio &a, const ratio &b );
ratio operator*( const ratio &a, int64_t b );
ratio operator*( int64_t a, const ratio &b );
ratio operator/( const ratio &a, const ratio &b );
ratio operator/( const ratio &a, int64_t b );
ratio operator/( int64_t a, const ratio &b );
/// @}

////////////////////////////////////////

/// @defgroup RatioComparison ratio related comparison routines
///
/// @{
inline constexpr bool operator==( const ratio &a, const ratio &b )
{
	return a.numerator() * b.denominator() == b.numerator() * a.denominator();
}

inline constexpr bool operator!=( const ratio &a, const ratio &b )
{
	return !( a == b );
}

inline constexpr bool operator<( const ratio &a, const ratio &b )
{
	return a.numerator() * b.denominator() < b.numerator() * a.denominator();
}

inline constexpr bool operator<=( const ratio &a, const ratio &b )
{
	return a.numerator() * b.denominator() <= b.numerator() * a.denominator();
}

inline constexpr bool operator>( const ratio &a, const ratio &b )
{
	return a.numerator() * b.denominator() > b.numerator() * a.denominator();
}

inline constexpr bool operator>=( const ratio &a, const ratio &b )
{
	return a.numerator() * b.denominator() >= b.numerator() * a.denominator();
}
/// @}

////////////////////////////////////////

/// @brief stream operator
inline std::ostream &operator<<( std::ostream &out, const ratio &r )
{
	out << r.numerator() << '/' << r.denominator();
	return out;
}

////////////////////////////////////////

}

