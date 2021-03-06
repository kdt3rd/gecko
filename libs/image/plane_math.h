// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include "scanline.h"

namespace engine
{
class registry;
}

////////////////////////////////////////

namespace image
{
///
/// See plane_ops.h for the ability to call these in normal code
///
/// declares the implementation following operators:
///
/// void assign_value( scanline &dest, float v );
///
/// void add_planeplane( scanline &dest, const scanline &srcA, const scanline &srcB );
/// void add_planenumber( scanline &dest, const scanline &srcA, float v );
///
/// void sub_planeplane( scanline &dest, const scanline &srcA, const scanline &srcB );
///
/// void mul_planeplane( scanline &dest, const scanline &srcA, const scanline &srcB );
/// void mul_planenumber( scanline &dest, const scanline &srcA, float v );
///
/// if srcB == 0 then out=0 else out = srcA/srcB
/// void div_planeplane( scanline &dest, const scanline &srcA, const scanline &srcB );
///
/// if src == 0 then out=0 else out = v/src
/// void div_numberplane( scanline &dest, float v, const scanline &src );
///
/// a * b + c
/// void muladd_planeplaneplane( scanline &dest, const scanline &srcA, const scanline &srcB, const scanline &srcC );
///
/// src * a + b
/// void muladd_planenumbernumber( scanline &dest, const scanline &src, float a, float b );
///
/// abs( src )
/// void plane_abs( scanline &dest, const scanline &src )
///
/// copysign( src, v )
/// void plane_copysign( scanline &dest, const scanline &src, const scanline &v )
///
/// src * src
/// void plane_square( scanline &dest, const scanline &src )
///
/// sqrt( src )
/// void plane_sqrt( scanline &dest, const scanline &src )
///
/// sqrt( srca*srca + srcb*srcb )
/// void plane_mag2( scanline &dest, const scanline &srcA, const scanline &srcB )
///
/// sqrt( srca*srca + srcb*srcb + srcc*srcc )
/// void plane_mag3( scanline &dest, const scanline &srcA, const scanline &srcB, const scanline &srcC )
///
///////// EXPONENTIAL FUNCTIONS
///
/// TODO: negative number handling?????
///
/// exp( src )
/// void plane_exp( scanline &dest, const scanline &src )
///
/// log( src ) (ln)
/// void plane_log( scanline &dest, const scanline &src )
///
/// log1p( src ) (ln(1+src))
/// void plane_log1p( scanline &dest, const scanline &src )
/// expm1( src ) (exp(src)-1)
/// void plane_log1p( scanline &dest, const scanline &src )
///
/// exp2( src )
/// void plane_exp2( scanline &dest, const scanline &src )
///
/// log2( src )
/// void plane_log2( scanline &dest, const scanline &src )
///
/// pow( srcA, srcB )
/// void plane_powp( scanline &dest, const scanline &srcA, const scanline &srcB )
///
/// powi( srcA, int )
/// void plane_powi( scanline &dest, int v )
///
/// powf( srcA, float )
/// void plane_powf( scanline &dest, float v )
///
///////// TRIG FUNCTIONS
///
/// atan2( srcA, srcB )
/// void plane_atan2( scanline &dest, const scanline &srcA, const scanline &srcB )
///
///
///////// CONDITIONAL FUNCTIONS
///
/// (a < b) ? c : d
/// void if_less( scanline &dest, const scanline &a, float b, const scanline &c, const scanline &d )
/// void if_greater( scanline &dest, const scanline &a, float b, const scanline &c, const scanline &d )
///
/// (a > t) ? 1 : 0
/// void threshold( scanline &dest, const scanline &a, float t )
/// void threshold( scanline &dest, const scanline &a, const scanline &t )

void add_plane_math( engine::registry &r );

} // namespace image
