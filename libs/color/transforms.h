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

/// basic transforms that a color undergoes:
/// - conversion to/from non-linear encoding to linear RGB
///   * conversion from one non-linear encoding to another
///     ... do we always know when a rendering (or the inverse) should be applied?
///         PQ <-> HLG involves OOTF bits to be correct...
/// - projection / reinterpretation in a different set of primaries
///   * optional white adaptation (i.e. ICC specifies Bradford when converting to/from D50)
/// - conversion to alternate space for special purpose manipulation / measurement
///   (i.e. L*a*b* for similarity)
/// - rendering for display, (maybe destructively) applying rendering curve
///   * user look management / OCIO (i.e. 3D LUT)
///   * ACES (optional look xform -> RRT -> ODT)
///   * BT.2100 OOTF functions?
///   * BT.1886 from BT.709

/// color conversion between arbitrary states happens in a prescribed chain
/// 0. <remove any chroma subsample>
/// 1. convert any sub-range to full
/// 2. convert color opponent spaces back to (non-linear) RGB-ish (could be XYZ, etc.)
/// 3. remove any non-linear encoding
/// 4. apply any (inverse) rendering function
///    (which may have an intermediate working space to convert to)
/// 5. apply any matrix transforms or other tristimulus mixing
///    a. RGB/LMS/whatever to XYZ
///    b. color adaptation matrix
///    c. XYZ to RGB/LMS/whatever
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
/// transformation / matrix by incorporating offset and scale into the
/// equation for the color opponency, but are logically described as
/// separate steps here, and may be implemented separately for
/// understandability.
///
/// additionally, depending on source and destination state, any of
/// the inner steps where the resulting step would be an identity when
/// concatenated can be skipped for efficiency, but again, are not
/// skipped in this function for completeness, but would be expected
/// in an operation that is defined across a buffer of values.
///
template <typename T>
inline void convert( T &a, T &b, T &c, const state &from, const state &to, const int bits, cone_response cr = cone_response::NONE )
{
	// TODO: handle integral values and loss of precision
	to_full( a, b, c, a, b, c, from.current_space(), from.signal(), bits );
//	if ( has_opponency( from.current_space() ) )
//		remove_opponency( a, b, c, a, b, c, from.current_space() );
//	if ( from.curve() != to.curve() || !( to.is_same_matrix( from ) ) )
	if ( from.curve() != to.curve() )
	{
		triplet<T> v;
		v.x = linearize( a, from.curve() );
		v.y = linearize( b, from.curve() );
		v.z = linearize( c, from.curve() );
		matrix<T> m = to.get_from_xyz_mat() * to.adaptation( from, cr ) * from.get_to_xyz_mat();
		v = m * v;
		a = encode( v.x, to.curve() );
		b = encode( v.y, to.curve() );
		c = encode( v.z, to.curve() );
	}
//	add_opponency( a, b, c, a, b, c, to.current_space() );
	// TODO: should we clamp SDI values ever?
	from_full( a, b, c, a, b, c, to.current_space(), to.signal(), bits, false );
}

template <typename T, int fbits>
inline tristimulus_value<T, fbits> convert( const tristimulus_value<T, fbits> &v, const state &to )
{
	using component_type = T;
	using v_t = tristimulus_value<component_type, fbits>;
	v_t r{ v.x(), v.y(), v.z(), to };
	convert( r.x(), r.y(), r.z(), v.current_state(), v_t::valid_bits, to );
	return r;
}

/// short cut when you just want to convert the space
template <typename T, int fbits>
inline tristimulus_value<T, fbits> convert_space( const tristimulus_value<T, fbits> &v, space tospace )
{
	return convert( v, state{ v.current_state(), tospace } );
}

} // namespace color



