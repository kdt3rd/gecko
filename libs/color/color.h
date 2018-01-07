//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "triplet.h"
#include "state.h"
#include "standards.h"
#include <type_traits>

////////////////////////////////////////

namespace color
{

///
/// @brief Class color provides...
///
template <typename T, int forcebits = -1>
class tristimulus_value
{
	// TODO: allow a half data type - not sure is_arithmetic will return true for half?
	static_assert( std::is_arithmetic<T>::value, "tristimulus_value component type must be arithmetic" );
	static_assert( forcebits == -1 || ( forcebits > 0 && forcebits <= 8*sizeof(T) ), "invalid number of bits for tristimulus_value" );

public:
	using component_type = T;
	/// primarily for integer representations, will always be the MSB bits
	constexpr int valid_bits = ( forcebits > 0 ? forcebits : 8*sizeof(T) );

	constexpr tristimulus_value( void ) = default;
	constexpr ~tristimulus_value( void ) = default;
	tristimulus_value( const tristimulus_value & ) = default;
	constexpr tristimulus_value( tristimulus_value && ) = default;
	constexpr tristimulus_value &operator=( tristimulus_value && ) = default;

	
	constexpr tristimulus_value &operator=( const tristimulus_value & ) = default;
	template <typename To, int bO>
	tristimulus_value &operator=( const tristimulus_value<To, bO> &o )
	{
		_state = o._state;
		_state.bits( valid_bits );
		_x = convert_bits<To, bO, component_type, bits>( o._x );
		_y = convert_bits<To, bO, component_type, bits>( o._y );
		_z = convert_bits<To, bO, component_type, bits>( o._z );
		return *this;
	}

	// allow conversion of data type, but only explicitly...
	template <typename To, int bO = -1>
	explicit operator tristimulus_value<To, bO>( void ) const
	{
	}
	constexpr component_type x( void ) const { return _x; }
	constexpr component_type y( void ) const { return _y; }
	constexpr component_type z( void ) const { return _z; }

	void set( component_type a, component_type b, component_type c )
	{
		_x = a;
		_y = b;
		_z = c;
	}

	void convert( const state &s )
	{
		
	}

	tristimulus_value convert( const state &s ) const
	{
	}

	/// convenience for when you just want to change what space
	/// the current tristimulus value is in
	tristimulus_value convert( space s ) const
	{
	}

	double deltaE_1976( const tristimulus_value &o ) const;
	double deltaE_1994( const tristimulus_value &o ) const;
	double deltaE_2000( const tristimulus_value &o ) const;
	double deltaE_CMC( const tristimulus_value &o ) const;
	/// computes the euclidian distance in lab
	double deltaLAB( const tristimulus_value &o ) const;
	
	constexpr const state &current_state( void ) const { return _state; }

protected:
	state _state = { make_standard<standard::SRGB>() };
	component_type _x = component_type(0);
	component_type _y = component_type(0);
	component_type _z = component_type(0);
};

/// mixes two colors together in linear RGB
template <typename Ta, int bA, typename Tb, int bB>
tristimulus_value<typename std::common_type<Ta, Tb>::type, (bA <= bB) ? bB : bA>
mix( const tristimulus_value<Ta, int bA> &a, const tristimulus_value<Tb, int bB> &b, float m = 0.5F )
{
	
}

/// mixes two colors together in whatever current space they are in
/// NB: this is non-standard, hence the function name, but some programs need it for correct behavior
template <typename Ta, int bA, typename Tb, int bB>
tristimulus_value<typename std::common_type<Ta, Tb>::type, (bA <= bB) ? bB : bA>
unnorm_mix( const tristimulus_value<Ta, int bA> &a, const tristimulus_value<Tb, int bB> &b, float m = 0.5F )
{
	
}
	

template <typename T, int forcebits = -1>
class value_with_alpha
{
public:
	using component_type = T;
	using value_type = tristimulus_value<T>;

	constexpr value_with_alpha( void ) = default;
	constexpr ~value_with_alpha( void ) = default;
	value_with_alpha( const value_with_alpha & ) = default;
	value_with_alpha &operator=( const value_with_alpha & ) = default;
	constexpr value_with_alpha( value_with_alpha && ) = default;
	constexpr value_with_alpha &operator=( value_with_alpha && ) = default;

	value &v( void ) { return _v; }
	const value &v( void ) const { return _v; }
	component_type &a( void ) { return _a; }
	component_type a( void ) const { return _a; }

	void set( component_type x, component_type y, component_type z, component_type a = component_type(1) )
	{
		_v.set( x, y, z );
		_a = a;
	}

private:
	value_type _v;
	component_type _a = component_type(1);
};

using color8b = tristimulus_value<uint8_t>;
using color10b = tristimulus_value<uint16_t, 10>;
using color12b = tristimulus_value<uint16_t, 12>;
using color14b = tristimulus_value<uint16_t, 14>;
using color16b = tristimulus_value<uint16_t>;

using colorf = tristimulus_value<float>;
using color4f = value_with_alpha<float>;

using colord = tristimulus_value<double>;
using color4d = value_with_alpha<double>;

} // namespace color



