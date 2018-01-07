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
inline tristimulus_value<T> convert( const tristimulus_value<T> &v, const state &from, const state &to )
{
}

/// short cut when you just want to convert the space
template <typename T>
inline tristimulus_value<T> convert_space( const tristimulus_value<T> &v, const state &s, space tospace )
{
	return convert( v, s, state{ s, tospace } );
}

template <typename T>
inline tristimulus_value<T> desaturate( const tristimulus_value<T> &v, T amt )
{
	using v_t = tristimulus_value<T>;
	v_t lab = v;
	auto lab = convert_space( v, s, space::CIE_LAB_76 );
	lab.y() *= amt;
	lab.z() *= amt;
	return convert( labs, s, lab );
}

template <typename T>
inline T distance( const state &s, const tristimulus_value<T> &a, const tristimulus_value<T> &b )
{
	state labs = s;
	labs.space( space::CIE_LAB_76 );
	auto lab = convert( s, labs, v );
	lab.y() *= amt;
	lab.z() *= amt;
	return convert( labs, s, lab );
}

template <typename T>
inline T distance( const state &s, const tristimulus_value<T> &a, const tristimulus_value<T> &b )
{
	state labs = s;
	labs.space( space::CIE_LAB_76 );
	auto lab = convert( s, labs, v );
	lab.y() *= amt;
	lab.z() *= amt;
	return convert( labs, s, lab );
}

} // namespace color



