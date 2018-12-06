//
// Copyright (c) 2017 Kimball Thurston
// SPDX-License-Identifier: MIT
// Copyrights licenced under the MIT License.
//

#pragma once

#include <vector>
#include <type_traits>
#include <limits>
#include <initializer_list>
#include <base/math_functions.h>
#include <base/contract.h>

////////////////////////////////////////

namespace color
{

///
/// @brief Class spectral_density provides...
///
template <typename T>
class spectral_density
{
	static_assert( std::is_floating_point<T>::value, "expect floating point value for type" );

public:
	using value_type = T;
	using wavelength_table = std::vector<value_type>;

	spectral_density( void ) = default;
	spectral_density( const spectral_density & ) = default;
	spectral_density( spectral_density && ) = default;
	spectral_density &operator=( const spectral_density & ) = default;
	spectral_density &operator=( spectral_density && ) = default;
	~spectral_density( void ) = default;

	spectral_density( value_type start, value_type delta, std::initializer_list<value_type> l )
		: _table( l ), _lambda_s( start ), _lambda_d( delta )
	{}
		
	void reset( size_t nEntries, value_type start, value_type delta )
	{
		wavelength_table nT( nEntries, value_type(0) );
		_table.swap( nT );
		_lambda_s = start;
		_lambda_d = delta;
	}

	/// creates a new spectral_density item, but with values in the table == 0, but
	/// with the same size and wavelength range as this current spectral_density
	spectral_density clone( void ) const
	{
		spectral_density r;
		r.reset( size(), start_wavelength(), delta() );
		return r;
	}

	spectral_density reinterpret( size_t nEntries, value_type start, value_type delta ) const
	{
		spectral_density r;
		r.reset( nEntries, start, delta );
		value_type curL = start;
		for ( size_t i = 0; i != nEntries; ++i, curL += delta )
			r[i] = sample( curL, delta );
		return r;
	}

	bool empty( void ) const { return _table.empty(); }
	bool valid( void ) const { return ! empty() && delta() > value_type(0); }

	size_t size( void ) const { return _table.size(); }

	value_type start_wavelength( void ) const { return _lambda_s; }
	value_type delta( void ) const { return _lambda_d; }

	inline value_type operator()( value_type l ) const { return sample( l ); }
	inline value_type sample( value_type l ) const
	{
		precondition( valid(), "sampling invalid spectral density table" );
		value_type idxF = ( l - start_wavelength() ) / delta();
		long idx = static_cast<long>( idxF );
		value_type ret = value_type(0);
		if ( idx >= 0 && idx < static_cast<long>( size() ) )
		{
			value_type a = _table[static_cast<size_t>( idx )];
			long idxp1 = idx + 1;
			value_type b = value_type(0);
			if ( idxp1 < static_cast<long>( size() ) )
				b = _table[static_cast<size_t>( idxp1 )];
			value_type mixV = idxF - static_cast<value_type>( idx );
			return base::lerp( a, b, mixV );
		}
		return ret;
	}

	/// produces an interpolated sample, allowing under / over
	/// sampling to produce linearly interpolated results, assuming
	/// center sampled results
	value_type sample( value_type l, value_type d ) const
	{
		precondition( d > value_type(0), "sampling invalid wavelength delta" );
		precondition( valid(), "sampling invalid spectral density table" );
		value_type ratio = d / delta();
		if ( ratio < value_type(1) )
			return sample( l );

		// undersampling, need to accumulate a few samples.
		// TODO: do we need a better curve than simple average?
		long nSamples = static_cast<long>( std::ceil( ratio ) );
		if ( nSamples % 2 == 0 )
			++nSamples;
		value_type radius = static_cast<value_type>( nSamples ) / value_type(2);

		value_type ret = value_type(0);
		for ( long i = 0; i < nSamples; ++i, curL += delta() )
		{
			value_type x = ( static_cast<value_type>( i ) - radius + value_type(0.5) );
			ret += sample( l + x * delta() );
		}
		return ret / static_cast<value_type>( nSamples );
	}

	value_type &operator[]( size_t i ) { return _table[i]; }
	value_type operator[]( size_t i ) const { return _table[i]; }

	wavelength_table &density( void ) { return _table; }
	const wavelength_table &density( void ) const { return _table; }

	template <typename OT>
	spectral_density &operator+=( const spectral_density<OT> &o )
	{
		value_type curL = start_wavelength();
		for ( size_t i = 0; i != size(); ++i, curL += delta() )
			_table[i] += o.sample( curL, delta() );
		return *this;
	}

	template <typename OT>
	typename std::common_type<value_type, typename OT::value_type>::type
	dot( const spectral_density<OT> &o )
	{
		using rettype = typename std::common_type<value_type, typename OT::value_type>::type;
		rettype ret = rettype(0);
		for ( size_t i = 0; i != size(); ++i, curL += delta() )
			ret += _table[i] * o.sample( curL, delta() );
		return ret;
	}

private:
	wavelength_table _table;
	value_type _lambda_s = value_type(0);
	value_type _lambda_d = value_type(0);
};

using spectral_density_f = spectral_density<float>;
using spectral_density_d = spectral_density<double>;

} // namespace color



