//
// Copyright (c) 2015-2016 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT
//

#include "ratio.h"
#include "math_functions.h"
#include "contract.h"

namespace base
{

////////////////////////////////////////

ratio::ratio( int64_t n, int64_t d, bool s )
		: _num( n ), _den( d )
{
	if ( s )
		simplify();
}

////////////////////////////////////////

void ratio::set( int64_t n, int64_t d )
{
	_num = n;
	_den = d;
	simplify();
}

////////////////////////////////////////

ratio
ratio::common( const ratio &r ) const
{
	precondition( valid() && r.valid(), "unable to find common denominator for invalid ratios" );

	if ( denominator() == r.denominator() )
		return *this;

	// find a / b ==> c / d  where d == r.denominator
	// d == lcm( r.denominator, b )
	int64_t d = lcm( denominator(), r.denominator() );
	return ratio( numerator() * d, denominator() * d, false );
}

////////////////////////////////////////

ratio &ratio::operator +=( const ratio &r )
{
	if ( denominator() == r.denominator() )
	{
		_num += r.numerator();
	}
	else
	{
		std::pair<ratio,ratio> c = rebase( *this, r );

		_num = c.first.numerator() + c.second.numerator();
		_den = c.first.denominator();
	}
	simplify();
	return *this;
}

////////////////////////////////////////

ratio &ratio::operator -=( const ratio &r )
{
	if ( denominator() == r.denominator() )
	{
		_num -= r.numerator();
	}
	else
	{
		std::pair<ratio,ratio> c = rebase( *this, r );

		_num = c.first.numerator() - c.second.numerator();
		_den = c.first.denominator();
	}
	simplify();
	return *this;
}

////////////////////////////////////////

ratio &ratio::operator *=( const ratio &r )
{
	_num *= r.numerator();
	_den *= r.denominator();
	simplify();
	return *this;
}

////////////////////////////////////////

ratio &ratio::operator /=( const ratio &r )
{
	_num *= r.denominator();
	_den *= r.numerator();
	simplify();
	return *this;
}

////////////////////////////////////////

ratio &ratio::operator +=( int64_t n )
{
	if ( denominator() == 1 )
		_num += n;
	else
	{
		_num += n * denominator();
		simplify();
	}
	return *this;
}

////////////////////////////////////////

ratio &ratio::operator -=( int64_t n )
{
	if ( denominator() == 1 )
		_num -= n;
	else
	{
		_num -= n * denominator();
		simplify();
	}
	return *this;
}

////////////////////////////////////////

ratio &ratio::operator *=( int64_t n )
{
	_num *= n;
	simplify();
	return *this;
}

////////////////////////////////////////

ratio &ratio::operator /=( int64_t n )
{
	_den *= n;
	simplify();
	return *this;
}

////////////////////////////////////////

void ratio::simplify( void )
{
	int64_t d = gcd( _num, _den );
	if ( d > 1 )
	{
		_num /= d;
		_den /= d;
	}
}

////////////////////////////////////////

std::pair<ratio,ratio> rebase( const ratio &a, const ratio &b )
{
	precondition( a.valid() && b.valid(), "unable to find common denominator for invalid ratios" );

	if ( a.denominator() == b.denominator() )
		return std::make_pair( a, b );

	if ( a.denominator() == 1 )
		return std::make_pair( ratio( a.numerator() * b.denominator(), b.denominator(), false ),
							   ratio( b.numerator(), b.denominator(), false ) );

	if ( b.denominator() == 1 )
		return std::make_pair( ratio( a.numerator(), a.denominator(), false ),
							   ratio( b.numerator() * a.denominator(), a.denominator(), false ) );

	int64_t d = lcm( a.denominator(), b.denominator() );

	// constructor will simplify to prevent
	// numbers growing
	return std::make_pair( ratio( a.numerator() * ( d / a.denominator() ), d, false ),
						   ratio( b.numerator() * ( d / b.denominator() ), d, false ) );
}

////////////////////////////////////////

ratio operator+( const ratio &a, const ratio &b )
{
	ratio t( a );
	t += b;
	return t;
}

ratio operator+( const ratio &a, int64_t b )
{
	ratio t( a );
	t += b;
	return t;
}

ratio operator+( int64_t a, const ratio &b )
{
	ratio t( b );
	t += a;
	return t;
}

ratio operator-( const ratio &a, const ratio &b )
{
	ratio t( a );
	t -= b;
	return t;
}

ratio operator-( const ratio &a, int64_t b )
{
	ratio t( a );
	t -= b;
	return t;
}

ratio operator-( int64_t a, const ratio &b )
{
	ratio t( a, int64_t(1) );
	t -= b;
	return t;
}

ratio operator*( const ratio &a, const ratio &b )
{
	ratio t( a );
	t *= b;
	return t;
}

ratio operator*( const ratio &a, int64_t b )
{
	ratio t( a );
	t *= b;
	return t;
}

ratio operator*( int64_t a, const ratio &b )
{
	ratio t( b );
	t *= a;
	return t;
}

ratio operator/( const ratio &a, const ratio &b )
{
	ratio t( a );
	t /= b;
	return t;
}

ratio operator/( const ratio &a, int64_t b )
{
	ratio t( a );
	t /= b;
	return t;
}

ratio operator/( int64_t a, const ratio &b )
{
	ratio t( a, int64_t(1) );
	t /= b;
	return t;
}

////////////////////////////////////////

}

