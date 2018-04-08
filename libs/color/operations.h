//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "transforms.h"

////////////////////////////////////////

namespace color
{

/// mixes two colors together in whatever current space they are in
/// NB: this is non-standard, hence the function name, but some programs need it for correct behavior
template <typename Ta, int bA, typename Tb, int bB>
tristimulus_value<typename std::common_type<Ta, Tb>::type, (bA <= bB) ? bB : bA>
unnorm_mix( const tristimulus_value<Ta, bA> &a, const tristimulus_value<Tb, bB> &b, float m = 0.5F )
{
	precondition( a.current_state() == b.current_state(), "two colors should be in same state to mix" );
	using result_comp = typename std::common_type<Ta, Tb>::type;
	using result_t = tristimulus_value<result_comp, (bA <= bB) ? bB : bA>;
	using mix_t = typename std::common_type<float, result_comp>::type;
	const mix_t mm = static_cast<mix_t>( m );
	const mix_t n = mix_t(1) - mm;
	return result_t( static_cast<result_comp>( static_cast<mix_t>( a.r() ) * mm +
											   static_cast<mix_t>( b.r() ) * n ),
					 static_cast<result_comp>( static_cast<mix_t>( a.g() ) * mm +
											   static_cast<mix_t>( b.g() ) * n ),
					 static_cast<result_comp>( static_cast<mix_t>( a.b() ) * mm +
											   static_cast<mix_t>( b.b() ) * n ),
					 a.current_state() );
}

/// mixes two colors together in linear RGB
template <typename Ta, int bA, typename Tb, int bB>
tristimulus_value<typename std::common_type<Ta, Tb>::type, (bA <= bB) ? bB : bA>
mix( const tristimulus_value<Ta, bA> &a, const tristimulus_value<Tb, bB> &b, float m = 0.5F )
{
	state mixs = a.current_state();
	mixs.current_space( space::RGB );
	mixs.curve( transfer::LINEAR );
	return unnorm_mix( convert( a, mixs ), convert( b, mixs ) );
}

template <typename T, int fbits>
inline tristimulus_value<T> desaturate( const tristimulus_value<T, fbits> &v, T amt, bool return_to_orig = true )
{
	auto lab = convert_space( v, space::CIE_LAB_76 );
	lab.y() *= amt;
	lab.z() *= amt;
	return ( return_to_orig ) ? convert( lab, lab.current_state(), v.current_state() ) : lab;
}

template <typename T, int fbits>
inline T distance( const tristimulus_value<T, fbits> &a, const tristimulus_value<T, fbits> &b )
{
	auto labA = convert_space( a, space::CIE_LAB_76 );
	auto labB = convert_space( b, space::CIE_LAB_76 );
	T dL = labA.x() - labB.x();
	T da = labA.y() - labB.y();
	T db = labA.z() - labB.z();
	return std::sqrt( dL * dL + da * da + db * db );
}

} // namespace color



