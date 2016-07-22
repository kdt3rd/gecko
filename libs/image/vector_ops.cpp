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

#include "vector_ops.h"
#include "threading.h"
#include "scanline_process.h"
#include <base/cpu_features.h>
#include <base/contract.h>

////////////////////////////////////////

namespace
{
using namespace image;

static void
cvtToAbsU( scanline &dest, const scanline &u )
{
	for ( int x = 0; x < dest.width(); ++x )
		dest[x] = u[x] - static_cast<float>( x );
}

static void
cvtToAbsV( scanline &dest, int y, const plane &v )
{
	const float *vLine = v.line( y );
	float yOff = static_cast<float>( y );
	for ( int x = 0; x < dest.width(); ++x )
		dest[x] = vLine[x] - yOff;
}

static void
cvtToRelU( scanline &dest, const scanline &u )
{
	for ( int x = 0; x < dest.width(); ++x )
		dest[x] = u[x] + static_cast<float>( x );
}

static void
cvtToRelV( scanline &dest, int y, const plane &v )
{
	const float *vLine = v.line( y );
	float yOff = static_cast<float>( y );
	for ( int x = 0; x < dest.width(); ++x )
		dest[x] = vLine[x] + yOff;
}

////////////////////////////////////////

static void
applyWarpBilinearP( scanline &dest, int y, const plane &src, const vector_field &v, bool isAbsolute )
{
	if ( isAbsolute )
	{
		const plane::value_type *uLine = v.u().line( y );
		const plane::value_type *vLine = v.v().line( y );
		for ( int x = 0; x < dest.width(); ++x )
		{
			plane::value_type uV = uLine[x];
			plane::value_type vV = vLine[x];
			dest[x] = bilinear_hold( src, uV, vV );
		}
	}
	else
	{
		const plane::value_type *uLine = v.u().line( y );
		const plane::value_type *vLine = v.v().line( y );
		plane::value_type yOff = static_cast<plane::value_type>( y );
		for ( int x = 0; x < dest.width(); ++x )
		{
			plane::value_type uV = uLine[x] + static_cast<plane::value_type>( x );
			plane::value_type vV = vLine[x] + yOff;
			dest[x] = bilinear_hold( src, uV, vV );
		}
	}
}

////////////////////////////////////////

static void
applyWarpDiracP( scanline &dest, int y, const plane &src, const vector_field &v, bool isAbsolute )
{
	if ( isAbsolute )
	{
		const plane::value_type *uLine = v.u().line( y );
		const plane::value_type *vLine = v.v().line( y );
		for ( int x = 0; x < dest.width(); ++x )
		{
			plane::value_type uV = uLine[x];
			plane::value_type vV = vLine[x];
			
			dest[x] = get_hold( src, static_cast<int>( uV ),
								static_cast<int>( vV ) );
		}
	}
	else
	{
		const plane::value_type *uLine = v.u().line( y );
		const plane::value_type *vLine = v.v().line( y );
		for ( int x = 0; x < dest.width(); ++x )
		{
			dest[x] = get_hold( src, static_cast<int>( uLine[x] ) + x,
								static_cast<int>( vLine[x] ) + y );
		}
	}
}

} // empty namespace

////////////////////////////////////////

namespace image
{

plane warp_bilinear( const plane &src, const vector_field &v, bool isAbsolute )
{
	precondition( v.width() == src.width() && v.height() == src.height(), "Vector field not same size as plane requested for warp" );
	return plane( "v.warp_bilinear", src.dims(), src, v, isAbsolute );
}

plane warp_dirac( const plane &src, const vector_field &v, bool isAbsolute )
{
	precondition( v.width() == src.width() && v.height() == src.height(), "Vector field not same size as plane requested for warp" );
	return plane( "v.warp_dirac", src.dims(), src, v, isAbsolute );
}

vector_field convert_to_absolute( const vector_field &v )
{
	return vector_field::create(
		plane( "v.cvt_to_abs_u", v.u().dims(), v.u() ),
		plane( "v.cvt_to_abs_v", v.v().dims(), v.v() )
								);
}

////////////////////////////////////////

vector_field convert_to_relative( const vector_field &v )
{
	return vector_field::create(
		plane( "v.cvt_to_rel_u", v.u().dims(), v.u() ),
		plane( "v.cvt_to_rel_v", v.v().dims(), v.v() )
								);
}

////////////////////////////////////////

void add_vector_ops( engine::registry &r )
{
	using namespace engine;

	r.add( op( "v.warp_bilinear", base::choose_runtime( applyWarpBilinearP ), n_scanline_plane_adapter<false, decltype(applyWarpBilinearP)>(), dispatch_scan_processing, op::n_to_one ) );

	r.add( op( "v.warp_dirac", base::choose_runtime( applyWarpDiracP ), n_scanline_plane_adapter<false, decltype(applyWarpDiracP)>(), dispatch_scan_processing, op::n_to_one ) );

	r.add( op( "v.cvt_to_abs_u", base::choose_runtime( cvtToAbsU ), scanline_plane_adapter<true, decltype(cvtToAbsU)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "v.cvt_to_abs_v", base::choose_runtime( cvtToAbsV ), n_scanline_plane_adapter<false, decltype(cvtToAbsV)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "v.cvt_to_rel_u", base::choose_runtime( cvtToRelU ), scanline_plane_adapter<true, decltype(cvtToRelU)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "v.cvt_to_rel_v", base::choose_runtime( cvtToRelV ), n_scanline_plane_adapter<false, decltype(cvtToRelV)>(), dispatch_scan_processing, op::n_to_one ) );
}

////////////////////////////////////////

} // namespace image

