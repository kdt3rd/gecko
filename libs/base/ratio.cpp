
#include "ratio.h"
#include "math_functions.h"
#include "contract.h"

namespace base
{

////////////////////////////////////////

ratio::ratio( int64_t n, int64_t d )
		: _num( n ), _den( d )
{
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

std::pair<ratio,ratio> ratio::common( const ratio &r ) const
{
	precondition( valid() && r.valid(), "unable to find common denominator for invalid ratios" );

	int64_t newnumA = math::gcd( numerator() * r.denominator(), r.numerator() * denominator() );
	int64_t newnumB = math::gcd( r.numerator() * denominator(), numerator() * r.denominator() );
	int64_t newden = denominator() * r.denominator();

	// constructor will simplify to prevent
	// numbers growing
	return std::make_pair( ratio( newnumA, newden ),
						   ratio( newnumB, newden ) );
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
		std::pair<ratio,ratio> c = common( r );

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
		std::pair<ratio,ratio> c = common( r );

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
	int64_t d = math::gcd( _num, _den );
	if ( d > 1 )
	{
		_num /= d;
		_den /= d;
	}
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

