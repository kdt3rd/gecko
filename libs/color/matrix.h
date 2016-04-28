//
// Copyright (c) 2016 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#pragma once

#include <array>
#include <limits>
#include <stdexcept>
#include <ostream>
#include "triplet.h"

////////////////////////////////////////

namespace color
{

///
/// @brief Class matrix provides a simple 3x3 matrix
///
/// This is not designed as a general matrix, but instead specific for
/// doing color manipulations.
template <typename V>
class matrix
{
	static_assert( std::is_floating_point<V>::value, "color::matrix requires floating point type" );
public:
	typedef V value_type;

	inline constexpr matrix( void ) noexcept : _m{ value_type(1), value_type(0), value_type(0), value_type(0), value_type(1), value_type(0), value_type(0), value_type(0), value_type(1) }
	{}
	/// constructs a diagonal matrix
	inline constexpr matrix( value_type a, value_type b, value_type c ) noexcept : _m{ a, value_type(0), value_type(0), value_type(0), b, value_type(0), value_type(0), value_type(0), c }
	{}
	inline constexpr matrix( std::initializer_list<value_type> l ) noexcept : _m(l) {}
	inline constexpr matrix( value_type m00, value_type m01, value_type m02, value_type m10, value_type m11, value_type m12, value_type m20, value_type m21, value_type m22 ) noexcept : _m{ m00, m01, m02, m10, m11, m12, m20, m21, m22 } {}
	inline constexpr matrix( const triplet<value_type> &c1, const triplet<value_type> &c2, const triplet<value_type> &c3 ) noexcept : _m{ c1.x, c2.x, c3.x, c1.y, c2.y, c3.y, c1.z, c2.z, c3.z } {}
	inline constexpr matrix( const matrix & ) = default;
	inline constexpr matrix( matrix && ) = default;
	inline matrix &operator=( const matrix & ) = default;
	inline matrix &operator=( matrix && ) = default;

	inline value_type *operator[]( int i ) { return _m[i*3]; }
	inline const value_type *operator[]( int i ) const { return _m[i*3]; }

	inline void identity( void ) noexcept
	{
		a() = value_type(1);
		b() = value_type(0);
		c() = value_type(0);
		d() = value_type(0);
		e() = value_type(1);
		f() = value_type(0);
		g() = value_type(0);
		h() = value_type(0);
		i() = value_type(1);
	}

	inline constexpr value_type determinant( void ) const noexcept
	{
		// det(A) = a(ei - fh) - b(di - fg) + c(dh -eg)
		return ( a()*(e()*i() - f()*h()) - b()*(d()*i() - f()*g()) + c()*(d()*h() - e()*g()) );
	}

	inline matrix invert( void ) const
	{
		// based on computationally efficient version on wikipedia
		value_type A = ( e()*i() - f()*h() );
		value_type B = ( f()*g() - d()*i() ); // -( di - fg );
		value_type C = ( d()*h() - e()*g() );
		value_type D = ( c()*h() - b()*i() ); // -( bi - ch )
		value_type E = ( a()*i() - c()*g() );
		value_type F = ( b()*g() - a()*h() ); // -(ah - bg)
		value_type G = ( b()*f() - c()*e() );
		value_type H = ( c()*d() - a()*f() ); // -(af - cd)
		value_type I = ( a()*e() - b()*d() );
		value_type det = a()*A + b()*B + c()+C;
		if ( std::abs(det) < std::numeric_limits<value_type>::epsilon() )
			throw std::runtime_error( "Unable to invert degenerate color matrix" );

		det = value_type(1.0) / det;
		return matrix{ A*det, D*det, G*det,
					   B*det, E*det, H*det,
					   C*det, F*det, I*det };
	}

	inline constexpr matrix transpose( void ) const noexcept
	{
		return matrix{ a(), d(), g(), b(), e(), h(), c(), f(), i() };
	}

	inline matrix &operator *=( const matrix &m )
	{
		value_type an = a() * m.a() + b() * m.d() + c() * m.g();
		value_type bn = a() * m.b() + b() * m.e() + c() * m.h();
		value_type cn = a() * m.c() + b() * m.f() + c() * m.i();
		value_type dn = d() * m.a() + e() * m.d() + f() * m.g();
		value_type en = d() * m.b() + e() * m.e() + f() * m.h();
		value_type fn = d() * m.c() + e() * m.f() + f() * m.i();
		value_type gn = g() * m.a() + h() * m.d() + i() * m.g();
		value_type hn = g() * m.b() + h() * m.e() + i() * m.h();
		value_type in = g() * m.c() + h() * m.f() + i() * m.i();
		a() = an;
		b() = bn;
		c() = cn;
		d() = dn;
		e() = en;
		f() = fn;
		g() = gn;
		h() = hn;
		i() = in;
	}

	static constexpr matrix diag( value_type a, value_type b, value_type c )
	{
		return matrix( a, b, c );
	}
	static constexpr matrix diag( const triplet<value_type> &t )
	{
		return matrix( t.x, t.y, t.z );
	}

private:
	// A = [a b c; d e f; g h i]
	inline constexpr value_type a( void ) const noexcept { return _m[0]; };
	inline value_type &a( void ) noexcept { return _m[0]; };
	inline constexpr value_type b( void ) const noexcept { return _m[1]; };
	inline value_type &b( void ) noexcept { return _m[1]; };
	inline constexpr value_type c( void ) const noexcept { return _m[2]; };
	inline value_type &c( void ) noexcept { return _m[2]; };

	inline constexpr value_type d( void ) const noexcept { return _m[3]; };
	inline value_type &d( void ) noexcept { return _m[3]; };
	inline constexpr value_type e( void ) const noexcept { return _m[4]; };
	inline value_type &e( void ) noexcept { return _m[4]; };
	inline constexpr value_type f( void ) const noexcept { return _m[5]; };
	inline value_type &f( void ) noexcept { return _m[5]; };

	inline constexpr value_type g( void ) const noexcept { return _m[6]; };
	inline value_type &g( void ) noexcept { return _m[6]; };
	inline constexpr value_type h( void ) const noexcept { return _m[7]; };
	inline value_type &h( void ) noexcept { return _m[7]; };
	inline constexpr value_type i( void ) const noexcept { return _m[8]; };
	inline value_type &i( void ) noexcept { return _m[8]; };

	std::array<value_type, 9> _m;
};


////////////////////////////////////////


template <typename V>
inline std::ostream &operator<<( std::ostream &o, const matrix<V> &m )
{
	o << "{ " << m[0][0] << ' ' << m[0][1] << ' ' << m[0][2] << "; " << m[1][0] << ' ' << m[1][1] << ' ' << m[1][2] << "; " << m[2][0] << ' ' << m[2][1] << ' ' << m[2][2] << " }";
	return o;
}

template <typename V>
inline matrix<V> operator*( const matrix<V> &a, const matrix<V> &b )
{
	matrix<V> ret = a;
	ret *= b;
	return ret;
}

template <typename V>
inline matrix<V> operator*( matrix<V> &&a, const matrix<V> &b )
{
	a *= b;
	return std::move( a );
}

/// @brief applies column-wise multiplication
///
/// [ x y z ] * A
template <typename V>
inline constexpr triplet<V> operator*( const triplet<V> &t, const matrix<V> &a ) noexcept
{
	return triplet<V>(
		a[0][0] * t.x + a[1][0] * t.y + a[2][0] * t.z,
		a[0][1] * t.x + a[1][1] * t.y + a[2][1] * t.z,
		a[0][2] * t.x + a[1][2] * t.y + a[2][2] * t.z );
}

/// @brief applies row-wise multiplication
///
/// A * [ x y z ]'
template <typename V>
inline constexpr triplet<V> operator*( const matrix<V> &a, const triplet<V> &t ) noexcept
{
	return triplet<V>(
		a[0][0] * t.x + a[0][1] * t.y + a[0][2] * t.z,
		a[1][0] * t.x + a[1][1] * t.y + a[1][2] * t.z,
		a[2][0] * t.x + a[2][1] * t.y + a[2][2] * t.z );
}

} // namespace color



