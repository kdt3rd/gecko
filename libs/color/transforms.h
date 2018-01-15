//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

////////////////////////////////////////

namespace color
{

/// color conversion between arbitrary states happens in a prescribed chain
/// 0. <remove any chroma subsample>
/// 1. convert any sub-range to full
/// 2. convert color opponent spaces back to (non-linear) RGB-ish (could be XYZ, etc.)
/// 3. remove any non-linear encoding
/// 4. apply any (inverse) rendering function
///    (which may have an intermediate working space to convert to)
/// 5. apply any matrix transforms
///    a. RGB to XYZ
///    b. color adaptation matrix
///    c. XYZ to RGB
///    d. (or other)
/// 6. apply any non-linear encoding
/// 7. convert to any color opponent space
/// 8. scale to any sub-range from full
/// 9. <add any output chroma subsampling>
///
/// steps 0 and 9 are outside the scope of this function, as they
/// imply area operations.
///
/// the above can be directly correlated to ICC profiles, where you
/// define your incoming or outgoing space, which allows ICC to apply
/// the input steps to convert to a connection space, then based on
/// the destination space, convert from the connection space to
/// destination, with the rendering intent in the middle.
///
/// steps (1 and 2) and (7 and 8) are usually combined into one
/// transformation by incorporating offset and scale into the equation
/// for the color opponency, but are logically described as separate
/// steps here, and may be implemented separately for
/// understandability.
///
/// additionally, depending on source and destination state, any of
/// the inner steps where the resulting step would be an identity when
/// concatenated can be skipped for efficiency, but again, are not
/// skipped in this function for completeness, but would be expected
/// in an operation that is defined across a buffer of values.
///
template <typename T>
inline void convert( T &a, T &b, T &c, const state &from, const state &to, int bits )
{
}

template <typename T, int fbits>
inline tristimulus_value<T, fbits> convert( const tristimulus_value<T, fbits> &v, const state &to )
{
	using component_type = T;
	using v_t = tristimulus_value<component_type, fbits>;
	v_t r{ v.x(), v.y(), v.z(), to };
	convert( r.x(), r.y(), r.z(), v.current_state(), to );
	return r;
}

/// short cut when you just want to convert the space
template <typename T, int fbits>
inline tristimulus_value<T, fbits> convert_space( const tristimulus_value<T, fbits> &v, space tospace )
{
	return convert( v, state{ v.current_state(), tospace } );
}

template <typename T, int fbits>
inline tristimulus_value<T> desaturate( const tristimulus_value<T> &v, T amt, bool return_to_orig = true )
{
	auto lab = convert_space( v, space::CIE_LAB_76 );
	lab.y() *= amt;
	lab.z() *= amt;
	return ( return_to_orig ) ? convert( lab, lab.current_state(), v.current_state() ) : lab;
}

template <typename T, int fbits>
inline T distance( const state &s, const tristimulus_value<T, fbits> &a, const tristimulus_value<T, fbits> &b )
{
	using v_t = tristimulus_value<T, fbits>;
	auto labA = convert_space( a, s, space::CIE_LAB_76 );
	state labs = s;
	labs.space( space::CIE_LAB_76 );
	auto lab = convert( s, labs, v );
	lab.y() *= amt;
	lab.z() *= amt;
	return convert( labs, s, lab );
}

template <typename T, int fbits>
inline T distance( const state &s, const tristimulus_value<T, fbits> &a, const tristimulus_value<T, fbits> &b )
{
	state labs = s;
	labs.space( space::CIE_LAB_76 );
	auto lab = convert( s, labs, v );
	lab.y() *= amt;
	lab.z() *= amt;
	return convert( labs, s, lab );
}

} // namespace color



