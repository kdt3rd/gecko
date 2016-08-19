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
#include <vector>
#include <limits>
#include <type_traits>
#include <cmath>
#include "contract.h"

////////////////////////////////////////

namespace math
{

template <typename T>
struct svd
{
	static_assert( std::is_arithmetic<T>::value, "svd requires arithmetic type for float_type" );

	typedef T float_type;

	typedef std::vector<float_type> col_type;
    typedef std::vector<col_type> grid_type;

	/// Initialize and decompose the given grid / matrix into
	/// (internal) diagonalized forms for use in subsequent solves.
	///
	/// if stable_eps > 0, performs a stable decomposition where small
	/// values are set to 0.
	///
	/// returns true if exact, false if the stable eps removed small
	/// values
	inline bool init( const grid_type &a, float_type stable_eps = float_type(1.0e-8) )
	{
		_u = a;
		return stable_decomp( stable_eps );
	}
	
	/// Same as above, but can hand off memory for incoming gride
	inline bool init( grid_type &&a, float_type stable_eps = float_type(1.0e-8) )
	{
		_u = std::move( a );
		return stable_decomp( stable_eps );
	}

	/// places the result in x
	inline void solve( const col_type &b, col_type &x ) const;

	inline const grid_type &u( void ) const { return _u; }
	inline const col_type &w( void ) const { return _w; }
	inline const grid_type &v( void ) const { return _v; }

private:
	void decomp( void );
	bool stable_decomp( float_type eps );

	grid_type _u;
	col_type _w;
	grid_type _v;
	mutable col_type _solve_tmp;

	static inline size_t cols( const grid_type &a ) { return a.empty() ? size_t(0) : a.front().size(); }
	static inline size_t rows( const grid_type &a ) { return a.size(); }
	static inline constexpr float_type square( float_type a ) { return a * a; }
	static inline float_type pythag( float_type a, float_type b )
	{
		float_type ret = float_type(0);
		float_type absa = std::abs( a );
		float_type absb = std::abs( b );
		if ( absa > absb )
			ret = absa * std::sqrt( float_type(1.0) + square( absb / absa ) );
		else if ( absb != float_type(0) )
			ret = absb * std::sqrt( float_type(1.0) + square( absa / absb ) );
		return ret;
	}
};

////////////////////////////////////////

template <typename T>
void
svd<T>::decomp( void )
{
	size_t n = cols( _u );
	size_t m = rows( _u );

	if ( n == 0 || m == 0 )
		throw_runtime( "Empty SVD grid to decompose" );

	float_type anorm, c, f, g, h, s, scale, x, y, z;
	col_type rv1;

	size_t l = 0;
	size_t nm = 0;

	_w.resize( n );
	rv1.resize( n );

	g = scale = anorm = float_type(0);

	//*** Householder reduction to bidiagonal form
	for ( size_t i = 0; i < n; ++i )
	{
		size_t l = i + 1;
		rv1[i] = scale * g;
		g = s = scale = float_type(0);

		if ( i < m )
		{
			for ( size_t k = i; k < m; ++k )
				scale += std::abs( _u[k][i] );

			if ( scale != float_type(0) )
			{
				for ( size_t k = i; k < m; ++k )
				{
					_u[k][i] /= scale;
					s += square( _u[k][i] );
				}

				f = _u[i][i];
				g = - std::copysign( std::sqrt( s ), f );
				h = f * g - s;
				_u[i][i] = f - g;

				for ( size_t j = l; j < n; ++j )
				{
					s = float_type(0);
					for ( size_t k = i; k < m; ++k )
						s += _u[k][i] * _u[k][j];

					f = s/h;

					for ( size_t k = i; k < m; ++k )
						_u[k][j] += f * _u[k][i];
				}

				for ( size_t k = i; k < m; ++k )
					_u[k][i] *= scale;
			}
		}

		_w[i] = scale * g;
		g = s = scale = float_type(0);

		if ( i < m && i != (n-1) )
		{
			for ( size_t k = l; k < n; ++k )
				scale += std::abs( _u[i][k] );

			if ( scale != float_type(0) )
			{
				for ( size_t k = l; k < n; ++k )
				{
					_u[i][k] /= scale;
					s += _u[i][k] * _u[i][k];
				}

				f = _u[i][l];
				g = -std::copysign( std::sqrt( s ), f );
				h = f * g - s;
				_u[i][l] = f - g;

				for ( size_t k = l; k < n; ++k )
					rv1[k] = _u[i][k]/h;

				for ( size_t j = l; j < m; ++j )
				{
					s = float_type(0);
					for ( size_t k = l; k < n; ++k )
						s += _u[j][k] * _u[i][k];

					for ( size_t k = l; k < n; ++k )
						_u[j][k] += s * rv1[k];
				}

				for ( size_t k = l; k < n; ++k )
					_u[i][k] *= scale;
			}
		}

		anorm = std::max( anorm,( std::abs( _w[i] ) +
								  std::abs( rv1[i] ) ) );
	}

	//*** Accumulation of right-hand transformations
	l = n - 1;
	_v[l][l] = float_type(1);
	g = rv1[l];
	for ( size_t i = l - 1; i < n; --i )
	{
		if ( g != float_type(0) )
		{
			for ( size_t j = l; j < n; ++j )
				_v[j][i] = ( _u[i][j] / _u[i][l] ) / g;

			for ( size_t j = l; j < n; ++j )
			{
				s = float_type(0);
				for ( size_t k = l; k < n; ++k )
					s += _u[i][k] * _v[k][j];

				for ( size_t k = l; k < n; ++k )
					_v[k][j] += s * _v[k][i];
			}
		}

		for ( size_t j = l; j < n; ++j )
			_v[i][j] = _v[j][i] = float_type(0);

		_v[i][i] = float_type(1);
		g = rv1[i];
		l = i;
	}

	//*** Acculmulation of the left-hand transformations
	for ( int lht = static_cast<int>( std::min( m, n ) - 1 ); lht >= 0; --lht )
	{
		size_t i = static_cast<size_t>( lht );
		l = i+1;
		g = _w[i];

		for ( size_t j = l; j < n; ++j )
			_u[i][j] = float_type(0);

		if ( g != float_type(0) )
		{
			g = float_type(1) / g;
			for ( size_t j = l; j < n; ++j )
			{
				s = float_type(0);
				for ( size_t k = l; k < m; ++k )
					s += _u[k][i] * _u[k][j];

				f = ( s/_u[i][i] ) * g;

				for ( size_t k = i; k < m; ++k )
					_u[k][j] += f * _u[k][i];
			}

			for ( size_t j = i; j < m; ++j )
				_u[j][i] *= g;
		}
		else
		{
			for ( size_t j = i; j < m; ++j )
				_u[j][i] = float_type(0);
		}

		_u[i][i]++;
	}

	//*** Diagonalization of the bidiagonal form
	for ( size_t k = n - 1; k < n; --k )
	{
		for ( size_t its = 0; its < 30; ++its )
		{
			bool flag = true;
			for ( l = k; l >= 0; --l )
			{
				nm = l - 1;

				if ( l == 0 || std::abs( rv1[l] ) <= std::numeric_limits<float_type>::epsilon() * anorm )
				{
					flag = false;
					break;
				}

				if ( std::abs( _w[nm] ) <= std::numeric_limits<float_type>::epsilon() * anorm )
					break;
			}

			if ( flag )
			{
				c = float_type(0);
				s = float_type(1);

				for ( size_t i = l; i <= k; ++i )
				{
					f = s * rv1[i];
					rv1[i] = c * rv1[i];

					if ( std::abs( f ) <= std::numeric_limits<float_type>::epsilon() * anorm )
						break;

					g = _w[i];
					h = pythag( f, g );
					_w[i] = h;
					h = float_type(1) / h;
					c = g * h;
					s = -f * h;

					for ( size_t j = 0; j < m; ++j )
					{
						y = _u[j][nm];
						z = _u[j][i];
						_u[j][nm] = y * c + z * s;
						_u[j][i] = z * c - y * s;
					}
				}
			}

			z = _w[k];

			if ( l == k )
			{
				if ( z < float_type(0) )
				{
					_w[k] = -z;
					for ( size_t j = 0; j < n; ++j )
						_v[j][k] = -_v[j][k];
				}
				break;
			}

			//*** Check for convergence
			if ( its == 29 )
			{
				throw_runtime( "No convergence after 30 SVD decomp iterations" );
				break;
			}

			x = _w[l];
			nm = k-1;
			y = _w[nm];
			g = rv1[nm];
			h = rv1[k];
			f = ( ( ( y - z ) * ( y + z ) + ( g - h ) * ( g + h ) ) /
				  ( float_type(2) * h * y ) );
			g = pythag( f, float_type(1) );
			f = ( ( x - z ) * ( x + z ) +
				  h * ( ( y / ( f + std::copysign( g, f ) ) ) - h ) ) / x;
			c = s = float_type(1);

			for ( size_t j = l; j <= nm; ++j )
			{
				size_t i = j + 1;
				g = rv1[i];
				y = _w[i];
				h = s * g;
				g = c * g;
				z = pythag( f, h );
				rv1[j] = z;
				c = f/z;
				s = h/z;
				f = x*c + g*s;
				g = g*c - x*s;
				h = y*s;
				y *= c;

				for ( size_t jj = 0; jj < n; ++jj )
				{
					x = _v[jj][j];
					z = _v[jj][i];
					_v[jj][j] = x*c + z*s;
					_v[jj][i] = z*c - x*s;
				}

				z = pythag( f, h );
				_w[j] = z;

				if ( z )
				{
					z = float_type(1) / z;
					c = f*z;
					s = h*z;
				}

				f = c*g + s*y;
				x = c*y - s*g;

				for ( size_t jj = 0; jj < m; ++jj )
				{
					y = _u[jj][j];
					z = _u[jj][i];
					_u[jj][j] = y*c + z*s;
					_u[jj][i] = z*c - y*s;
				}
			}

			rv1[l] = float_type(0);
			rv1[k] = f;
			_w[k] = x;
		}
	}
}

////////////////////////////////////////

template <typename T>
bool
svd<T>::stable_decomp( float_type eps )
{
	size_t N = cols( _u );
	float_type wmax = float_type(0);
	bool retval = true;

	_w.resize( N );
	_v.resize( N );
	for ( auto &vv: _v )
		vv.resize( N );

	// normal decomp
	decomp();

	if ( eps == float_type(0) )
		return retval;

	for ( auto wV: _w )
		wmax = std::max( wmax, wV );

	const float_type thresh = eps * wmax;

	for ( auto &wV: _w )
	{
		if ( wV < thresh )
		{
			wV = float_type(0);
			retval = false;
		}
	}

	return retval;
}

////////////////////////////////////////

template <typename T>
void
svd<T>::solve( const col_type &b, col_type &x ) const
{
	size_t n = cols( _u );
	size_t m = rows( _u );
	if ( n == 0 || m == 0 )
		throw_runtime( "Empty svd grid to solve" );

	x.resize( n );
	_solve_tmp.resize( n );

	for ( size_t j = 0; j < n; ++j )
	{
		float_type s = float_type(0);

		if ( _w[j] )
		{
			for ( size_t i = 0; i < m; ++i )
				s += _u[i][j] * b[i];

			s /= _w[j];
		}

		_solve_tmp[j] = s;
	}

	for ( size_t j = 0; j < n; ++j )
	{
		float_type s = float_type(0);

		for ( size_t jj = 0; jj < n; ++jj )
			s += _v[j][jj] * _solve_tmp[jj];

		x[j] = s;
	}
}

} // namespace math

////////////////////////////////////////

