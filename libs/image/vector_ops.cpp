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
#include "plane_ops.h"
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
applyWarpBilinearP( scanline &dest, int y, const plane &src, const vector_field &v )
{
	if ( v.is_absolute() )
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
applyWarpDiracP( scanline &dest, int y, const plane &src, const vector_field &v )
{
	if ( v.is_absolute() )
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

inline void colorize_hsv2rgb( float &r, float &g, float &b, float h, float s, float v )
{
	float c = v * s;
	float h2 = h * 6.F;
	float x = c * ( 1.F - fabsf( fmodf( h2, 2.F ) - 1.F ) );

	if ( 0.F <= h2 && h2 < 1.F )
	{ r = c; g = x; b = 0.F; }
	else if ( 1.F <= h2 && h2 < 2.F )
	{ r = x; g = c; b = 0.F; }
	else if ( 2.F <= h2 && h2 < 3.F )
	{ r = 0.F; g = c; b = x; }
	else if ( 3.F <= h2 && h2 < 4.F )
	{ r = 0.F; g = x; b = c; }
	else if ( 4.F <=h2 && h2 < 5.F)
	{ r = x; g = 0.F; b = c; }
	else if ( 5.F <=h2 && h2 <= 6.F )
	{ r = c; g = 0.F; b = x; }
	else if ( h2 > 6.F)
	{ r = 1.F; g = 0.F; b = 0.F; }
	else
	{ r = 0.F; g = 1.F; b = 0.F; }
}

static void colorize_thread_final( size_t, int s, int e, image_buf &ret, float scale, float maxMag )
{
	plane &red = ret[0];
	plane &green = ret[1];
	plane &blue = ret[2];
	plane &mag = ret[3];

	int w = ret.width();
	for ( int y = s; y < e; ++y )
	{
		float *rLine = red.line( y );
		float *gLine = green.line( y );
		float *bLine = blue.line( y );
		float *magLine = mag.line( y );
		for ( int x = 0; x < w; ++x )
		{
			float mV = magLine[x];
			float hue = rLine[x];
			float sat = std::min( std::max( mV * scale / maxMag, 0.F ), 1.F );
			float val = std::min( std::max( scale - sat, 0.F ), 1.F );

			colorize_hsv2rgb( rLine[x], gLine[x], bLine[x], hue, sat, val );
		}
	}
}

static void colorize_thread_abs( size_t tIdx, int s, int e, image_buf &ret, const vector_field &vec, std::vector<float> &mags )
{
	const plane &u = vec.u();
	const plane &v = vec.v();
	plane &red = ret[0];
	plane &mag = ret[3];

	float maxFlowMag = 0.F;

	int w = u.width();
	for ( int y = s; y < e; ++y )
	{
		const float *uLine = u.line( y );
		const float *vLine = v.line( y );
		float *rLine = red.line( y );
		float *magLine = mag.line( y );
		const float curY = static_cast<float>( y );
		for ( int x = 0; x < w; ++x )
		{
			float uV = uLine[x] - static_cast<float>( x );
			float vV = vLine[x] - curY;
			float mV = std::sqrt( uV * uV + vV * vV );
			magLine[x] = mV;
			maxFlowMag = std::max( maxFlowMag, mV );
			rLine[x] = fmodf( atan2f( vV, uV ) / ( 2.F * static_cast<float>( M_PI ) ) + 1.F, 1.F );
		}
	}

	mags[tIdx] = maxFlowMag;
}

static void colorize_thread_rel( size_t tIdx, int s, int e, image_buf &ret, const vector_field &vec, std::vector<float> &mags )
{
	const plane &u = vec.u();
	const plane &v = vec.v();
	plane &red = ret[0];
	plane &mag = ret[3];

	float maxFlowMag = 0.F;

	int w = u.width();
	for ( int y = s; y < e; ++y )
	{
		const float *uLine = u.line( y );
		const float *vLine = v.line( y );
		float *rLine = red.line( y );
		float *magLine = mag.line( y );
		for ( int x = 0; x < w; ++x )
		{
			float uV = uLine[x];
			float vV = vLine[x];
			float mV = std::sqrt( uV * uV + vV * vV );
			magLine[x] = mV;
			maxFlowMag = std::max( maxFlowMag, mV );
			float dir = fmodf( atan2f( vV, uV ) / ( 2.F * static_cast<float>( M_PI ) ) + 1.F, 1.F );

			rLine[x] = dir;
		}
	}

	mags[tIdx] = maxFlowMag;
}


static image_buf colorize_vector( const vector_field &v, float scale )
{
	std::vector<float> maxmags;
	maxmags.resize( threading::get().size(), 0.F );
	image_buf ret;
	ret.add_plane( plane( v.width(), v.height() ) );
	ret.add_plane( plane( v.width(), v.height() ) );
	ret.add_plane( plane( v.width(), v.height() ) );
	ret.add_plane( plane( v.width(), v.height() ) );

	if ( v.is_absolute() )
		threading::get().dispatch( std::bind( colorize_thread_abs, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( ret ), std::cref( v ), std::ref( maxmags ) ), 0, v.height() );
	else
		threading::get().dispatch( std::bind( colorize_thread_rel, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( ret ), std::cref( v ), std::ref( maxmags ) ), 0, v.height() );

	float maxMag = maxmags.front();
	for ( float mm: maxmags )
		maxMag = std::max( mm, maxMag );
	maxMag = std::max( 1.F, maxMag );

	threading::get().dispatch( std::bind( colorize_thread_final, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( ret ), scale, maxMag ), 0, v.height() );

	return ret;
}

} // empty namespace

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

plane confidence( const vector_field &a, const vector_field &b, int conservativeness )
{
	precondition( a.width() == b.width() && a.height() == b.height(), "Vector field not same size as other requested for divergence" );
	if ( conservativeness <= 0 )
		return log1p( magnitude( a.u() + warp_bilinear( b.u(), a ), a.v() + warp_bilinear( b.v(), a ) ) );

	return log1p( magnitude( a.u() + warp_bilinear( b.u(), a ), a.v() + warp_bilinear( b.v(), a ) ) *
				  magnitude( erode( a.u(), conservativeness ) - dilate( a.u(), conservativeness ),
							 erode( a.v(), conservativeness ) - dilate( a.v(), conservativeness ) ) );
}

////////////////////////////////////////

plane warp_dirac( const plane &src, const vector_field &v )
{
	precondition( v.width() == src.width() && v.height() == src.height(), "Vector field not same size as plane requested for warp" );
	return plane( "v.p.warp_dirac", src.dims(), src, v );
}

image_buf warp_dirac( const image_buf &src, const vector_field &v )
{
	image_buf ret;
	// anything to be gained from inlining this?
	for ( int i = 0; i != src.planes(); ++i )
		ret.add_plane( warp_dirac( src[i], v ) );
	return ret;
}

////////////////////////////////////////

plane warp_bilinear( const plane &src, const vector_field &v )
{
	precondition( v.width() == src.width() && v.height() == src.height(), "Vector field not same size as plane requested for warp" );
	return plane( "v.p.warp_bilinear", src.dims(), src, v );
}

image_buf warp_bilinear( const image_buf &src, const vector_field &v )
{
	image_buf ret;
	// anything to be gained from inlining this?
	for ( int i = 0; i != src.planes(); ++i )
		ret.add_plane( warp_bilinear( src[i], v ) );
	return ret;
}

vector_field convert_to_absolute( const vector_field &v )
{
	if ( v.is_absolute() )
		return vector_field::create( v.u(), v.v(), true );

	return vector_field::create(
		plane( "v.cvt_to_abs_u", v.u().dims(), v.u() ),
		plane( "v.cvt_to_abs_v", v.v().dims(), v.v() ),
		true
								);
}

////////////////////////////////////////

vector_field convert_to_relative( const vector_field &v )
{
	if ( v.is_absolute() )
		return vector_field::create(
			plane( "v.cvt_to_rel_u", v.u().dims(), v.u() ),
			plane( "v.cvt_to_rel_v", v.v().dims(), v.v() ),
			false );

	return vector_field::create( v.u(), v.v(), false );
}

////////////////////////////////////////

image_buf
colorize( const vector_field &v, float scale )
{
	engine::dimensions d;

	d.x = v.width();
	d.y = v.height();
	d.z = 4;

	return image_buf( "v.colorize", d, v, scale );
}

////////////////////////////////////////

void add_vector_ops( engine::registry &r )
{
	using namespace engine;

	r.add( op( "v.p.warp_dirac", base::choose_runtime( applyWarpDiracP ), n_scanline_plane_adapter<false, decltype(applyWarpDiracP)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "v.p.warp_bilinear", base::choose_runtime( applyWarpBilinearP ), n_scanline_plane_adapter<false, decltype(applyWarpBilinearP)>(), dispatch_scan_processing, op::n_to_one ) );


	r.add( op( "v.cvt_to_abs_u", base::choose_runtime( cvtToAbsU ), scanline_plane_adapter<true, decltype(cvtToAbsU)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "v.cvt_to_abs_v", base::choose_runtime( cvtToAbsV ), n_scanline_plane_adapter<false, decltype(cvtToAbsV)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "v.cvt_to_rel_u", base::choose_runtime( cvtToRelU ), scanline_plane_adapter<true, decltype(cvtToRelU)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "v.cvt_to_rel_v", base::choose_runtime( cvtToRelV ), n_scanline_plane_adapter<false, decltype(cvtToRelV)>(), dispatch_scan_processing, op::n_to_one ) );

	r.add( op( "v.colorize", colorize_vector, op::threaded ) );
}

////////////////////////////////////////

} // namespace image

