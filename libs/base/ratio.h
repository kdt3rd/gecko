
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
	ratio( int64_t n, int64_t d );
	ratio( const ratio & ) = default;
	ratio( ratio && ) = default;
	~ratio( void ) = default;
	ratio &operator=( const ratio & ) = default;
	ratio &operator=( ratio && ) = default;

	void set( int64_t n, int64_t d );

	inline bool valid( void ) const;

	inline int64_t numerator( void ) const;

	inline int64_t denominator( void ) const;

	inline double value( void ) const;

	inline ratio invert( void ) const;
	inline int64_t trunc( void ) const;
	inline int64_t mod( void ) const;
	inline int64_t round( void ) const;

	ratio &operator +=( const ratio &r );
	ratio &operator -=( const ratio &r );
	ratio &operator *=( const ratio &r );
	ratio &operator /=( const ratio &r );
	ratio &operator +=( int64_t n );
	ratio &operator -=( int64_t n );
	ratio &operator *=( int64_t n );
	ratio &operator /=( int64_t n );

	/// @brief Return a new pair of ratios with a common denominator
	std::pair<ratio,ratio> common( const ratio &r ) const;

private:
	void simplify( void );

	int64_t _num = 0;
	int64_t _den = 0;
};

////////////////////////////////////////

inline bool ratio::valid( void ) const
{
	return _den != 0;
}

inline int64_t ratio::numerator( void ) const
{
	return _num;
}

inline int64_t ratio::denominator( void ) const
{
	return _num;
}

inline double ratio::value( void ) const
{
	return static_cast<double>(_num) / static_cast<double>(_den);
}

inline ratio ratio::invert( void ) const
{
	return ratio( _den, _num );
}

inline int64_t ratio::trunc( void ) const
{
	return _num / _den;
}

inline int64_t ratio::mod( void ) const
{
	return _num % _den;
}

inline int64_t ratio::round( void ) const
{
	return ( _num + ( _num < 0 ? -_den : _den ) / 2 ) / _den;
}

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
inline bool operator==( const ratio &a, const ratio &b )
{
	return a.numerator() * b.denominator() == b.numerator() * a.denominator();
}

inline bool operator!=( const ratio &a, const ratio &b )
{
	return !( a == b );
}

inline bool operator<( const ratio &a, const ratio &b )
{
	return a.numerator() * b.denominator() < b.numerator() * a.denominator();
}

inline bool operator<=( const ratio &a, const ratio &b )
{
	return a.numerator() * b.denominator() <= b.numerator() * a.denominator();
}

inline bool operator>( const ratio &a, const ratio &b )
{
	return a.numerator() * b.denominator() > b.numerator() * a.denominator();
}

inline bool operator>=( const ratio &a, const ratio &b )
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

