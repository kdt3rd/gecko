//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "vector_ops.h"
#include "threading.h"
#include "scanline_process.h"
#include "plane_ops.h"
#include "plane_util.h"
#include <base/cpu_features.h>
#include <base/contract.h>
#include <cfloat>
#include <math.h>

////////////////////////////////////////

namespace
{
using namespace image;

static plane extract_u( const vector_field &v )
{
	return v.u();
}

////////////////////////////////////////

static plane extract_v( const vector_field &v )
{
	return v.v();
}

static vector_field modify_uv( const plane &u, const plane &v, bool a )
{
	return vector_field( u, v, a );
}

static void
cvtToAbsU( scanline &dest, const scanline &u )
{
	float offX = static_cast<float>( dest.offset() );
	for ( int x = 0; x < dest.width(); ++x )
		dest[x] = u[x] + static_cast<float>( x ) + offX;
}

static void
cvtToAbsV( scanline &dest, int y, const plane &v )
{
	const float *vLine = v.line( y );
	float yOff = static_cast<float>( y );
	for ( int x = 0; x < dest.width(); ++x )
		dest[x] = vLine[x] + yOff;
}

static void
cvtToRelU( scanline &dest, const scanline &u )
{
	float offX = static_cast<float>( dest.offset() );
	for ( int x = 0; x < dest.width(); ++x )
		dest[x] = u[x] - static_cast<float>( x ) - offX;
}

static void
cvtToRelV( scanline &dest, int y, const plane &v )
{
	const float *vLine = v.line( y );
	float yOff = static_cast<float>( y );
	for ( int x = 0; x < dest.width(); ++x )
		dest[x] = vLine[x] - yOff;
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
		plane::value_type xOff = static_cast<plane::value_type>( dest.offset() );
		for ( int x = 0; x < dest.width(); ++x )
		{
			plane::value_type uV = uLine[x] + static_cast<plane::value_type>( x ) + xOff;
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
		int offX = dest.offset();
		for ( int x = 0; x < dest.width(); ++x )
		{
			dest[x] = get_hold( src, static_cast<int>( uLine[x] ) + x + offX,
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

static void colorize_thread_final( size_t, int s, int e, image_buf &ret, float scale, float maxMag, float aveMag )
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
			float sat = std::min( std::max( mV * scale / aveMag, 0.F ), 1.F );
			float val = std::min( std::max( mV * scale / maxMag, 0.F ), 1.F );

			colorize_hsv2rgb( rLine[x], gLine[x], bLine[x], hue, sat, val );
		}
	}
}

static void colorize_thread_abs( size_t tIdx, int s, int e, image_buf &ret, const vector_field &vec, std::vector<float> &mags, std::vector<double> &avemags )
{
	const plane &u = vec.u();
	const plane &v = vec.v();
	plane &red = ret[0];
	plane &mag = ret[3];

	double maxFlowAve = 0.0;
	float maxFlowMag = 0.F;

	int w = u.width();
	float offx = static_cast<float>( u.x1() );
	for ( int y = s; y < e; ++y )
	{
		const float *uLine = u.line( y );
		const float *vLine = v.line( y );
		float *rLine = red.line( y );
		float *magLine = mag.line( y );
		const float curY = static_cast<float>( y );
		for ( int x = 0; x < w; ++x )
		{
			float uV = uLine[x] - static_cast<float>( x ) - offx;
			float vV = vLine[x] - curY;
			float mV = std::sqrt( uV * uV + vV * vV );
			magLine[x] = mV;
			maxFlowMag = std::max( maxFlowMag, mV );
			maxFlowAve += mV;
			rLine[x] = ( atan2f( vV, uV ) + static_cast<float>( M_PI ) ) / ( 2.F * static_cast<float>( M_PI ) );
		}
	}

	mags[tIdx] = maxFlowMag;
	avemags[tIdx] = maxFlowAve;
}

static void colorize_thread_rel( size_t tIdx, int s, int e, image_buf &ret, const vector_field &vec, std::vector<float> &mags, std::vector<double> &avemags )
{
	const plane &u = vec.u();
	const plane &v = vec.v();
	plane &red = ret[0];
	plane &mag = ret[3];

	double maxFlowAve = 0.0;
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
			maxFlowAve += mV;
			rLine[x] = ( atan2f( vV, uV ) + static_cast<float>( M_PI ) ) / ( 2.F * static_cast<float>( M_PI ) );
		}
	}

	mags[tIdx] = maxFlowMag;
	avemags[tIdx] = maxFlowAve;
}

static image_buf colorize_vector( const vector_field &v, float scale )
{
	std::vector<float> maxmags;
	std::vector<double> avemags;
	maxmags.resize( threading::get().size(), 0.F );
	avemags.resize( threading::get().size(), 0.0 );
	image_buf ret;
	ret.add_plane( plane( v.x1(), v.y1(), v.x2(), v.y2() ) );
	ret.add_plane( plane( v.x1(), v.y1(), v.x2(), v.y2() ) );
	ret.add_plane( plane( v.x1(), v.y1(), v.x2(), v.y2() ) );
	ret.add_plane( plane( v.x1(), v.y1(), v.x2(), v.y2() ) );

	if ( v.is_absolute() )
		threading::get().dispatch( std::bind( colorize_thread_abs, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( ret ), std::cref( v ), std::ref( maxmags ), std::ref( avemags ) ), v.y1(), v.height() );
	else
		threading::get().dispatch( std::bind( colorize_thread_rel, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( ret ), std::cref( v ), std::ref( maxmags ), std::ref( avemags ) ), v.y1(), v.height() );


	float maxMag = maxmags.front();
	for ( float mm: maxmags )
		maxMag = std::max( mm, maxMag );
	double aveMag = 0.0;
	for ( double mm: avemags )
		aveMag += mm;
	aveMag /= static_cast<double>( v.u().width() * v.u().height() );
	maxMag = std::max( 1.F, maxMag );
	std::cout << "color scaling by " << maxMag << " ave " << aveMag << std::endl;

	threading::get().dispatch( std::bind( colorize_thread_final, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( ret ), scale, maxMag, aveMag ), v.y1(), v.height() );

	return ret;
}

static void colorize_thread_rel_alpha( size_t tIdx, int s, int e, image_buf &ret, const vector_field &vec, const plane &alpha, std::vector<float> &mags, std::vector<double> &avemags, std::vector<double> &avesums )
{
	const plane &u = vec.u();
	const plane &v = vec.v();
	plane &red = ret[0];
	plane &mag = ret[3];

	double maxFlowAve = 0.0;
	float maxFlowMag = 0.F;
	double maxFlowAveSum = 0.0;

	int w = u.width();
	for ( int y = s; y < e; ++y )
	{
		const float *uLine = u.line( y );
		const float *vLine = v.line( y );
		const float *aLine = alpha.line( y );
		float *rLine = red.line( y );
		float *magLine = mag.line( y );
		for ( int x = 0; x < w; ++x )
		{
			if ( aLine[x] > 0.F )
			{
				float uV = uLine[x];
				float vV = vLine[x];
				float mV = std::sqrt( uV * uV + vV * vV );
				magLine[x] = mV;
				maxFlowMag = std::max( maxFlowMag, mV );
				maxFlowAve += mV;
				maxFlowAveSum += 1.0;
				rLine[x] = ( atan2f( vV, uV ) + static_cast<float>( M_PI ) ) / ( 2.F * static_cast<float>( M_PI ) );
			}
			else
			{
				magLine[x] = 0.F;
				rLine[x] = 0.F;
			}
		}
	}

	mags[tIdx] = maxFlowMag;
	avemags[tIdx] = maxFlowAve;
	avesums[tIdx] = maxFlowAveSum;
}

static image_buf colorize_vector_alpha( const vector_field &v, const plane &alpha, float scale )
{
	std::vector<float> maxmags;
	std::vector<double> avemags;
	std::vector<double> avesums;
	maxmags.resize( threading::get().size(), 0.F );
	avemags.resize( threading::get().size(), 0.0 );
	avesums.resize( threading::get().size(), 0.0 );
	image_buf ret;
	ret.add_plane( plane( v.x1(), v.y1(), v.x2(), v.y2() ) );
	ret.add_plane( plane( v.x1(), v.y1(), v.x2(), v.y2() ) );
	ret.add_plane( plane( v.x1(), v.y1(), v.x2(), v.y2() ) );
	ret.add_plane( plane( v.x1(), v.y1(), v.x2(), v.y2() ) );

	threading::get().dispatch( std::bind( colorize_thread_rel_alpha, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( ret ), std::cref( v ), std::cref( alpha ), std::ref( maxmags ), std::ref( avemags ), std::ref( avesums ) ), v.y1(), v.height() );


	float maxMag = maxmags.front();
	for ( float mm: maxmags )
		maxMag = std::max( mm, maxMag );
	double aveMag = 0.0;
	double aveSum = 0.0;
	for ( double mm: avemags )
		aveMag += mm;
	for ( double mm: avesums )
		aveSum += mm;

	if ( aveSum > 0.0 )
		aveMag /= aveSum;
	maxMag = std::max( 1.F, maxMag );
	std::cout << "color scaling by " << maxMag << " ave " << aveMag << std::endl;

	threading::get().dispatch( std::bind( colorize_thread_final, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( ret ), scale, maxMag, aveMag ), v.y1(), v.height() );

	return ret;
}

static void project_splat_bilinear( size_t, int s, int e, plane_buffer &outU, plane_buffer &outV, plane_buffer &splatCount, const const_plane_buffer &inU, const const_plane_buffer &inV, float scale )
{
	int w = outU.width();
	int offX = outU.x1();
	float srcOffScale = fabsf( scale );
	for ( int y = s; y < e; ++y )
	{
		const float *inUL = inU.line( y );
		const float *inVL = inV.line( y );
		for ( int x = 0; x < w; ++x )
		{
			float uV = inUL[x];
			float vV = inVL[x];
			float srcS = static_cast<float>( offX + x ) + uV * srcOffScale;
			float srcT = static_cast<float>( y ) + vV * srcOffScale;
			uV *= scale;
			vV *= scale;

			int ix, ix2, iy, iy2;
			float percX = bilinCoord( ix, ix2, srcS );
			float percY = bilinCoord( iy, iy2, srcT );
			if ( outU.in_bounds( ix, iy ) )
			{
				float splatV = ( 1.F - percX ) * ( 1.F - percY );
				outU.get( ix, iy ) += uV * splatV;
				outV.get( ix, iy ) += vV * splatV;
				splatCount.get( ix, iy ) += splatV;
			}
			if ( outU.in_bounds( ix2, iy ) )
			{
				float splatV = percX * ( 1.F - percY );
				outU.get( ix2, iy ) += uV * splatV;
				outV.get( ix2, iy ) += vV * splatV;
				splatCount.get( ix2, iy ) += splatV;
			}
			if ( outU.in_bounds( ix, iy2 ) )
			{
				float splatV = ( 1.F - percX ) * percY;
				outU.get( ix, iy2 ) += uV * splatV;
				outV.get( ix, iy2 ) += vV * splatV;
				splatCount.get( ix, iy2 ) += splatV;
			}
			if ( outU.in_bounds( ix2, iy2 ) )
			{
				float splatV = percX * percY;
				outU.get( ix2, iy2 ) += uV * splatV;
				outV.get( ix2, iy2 ) += vV * splatV;
				splatCount.get( ix2, iy2 ) += splatV;
			}
		}
	}
}

////////////////////////////////////////

static void project_final( size_t, int s, int e, plane_buffer &outU, plane_buffer &outV, const plane_buffer &splatCount )
{
	int w = outU.width();
	for ( int y = s; y < e; ++y )
	{
		float *outUL = outU.line( y );
		float *outVL = outV.line( y );
		const float *splatL = splatCount.line( y );
		for ( int x = 0; x < w; ++x )
		{
			float splatC = splatL[x];
			if ( splatC > 0.F )
			{
				outUL[x] /= splatC;
				outVL[x] /= splatC;
			}
		}
	}
}

static vector_field project_vector( const vector_field &v, float scale )
{
	plane accumU( v.x1(), v.y1(), v.x2(), v.y2() );
	memset( accumU.data(), 0, accumU.buffer_size() );
	plane accumV = accumU.copy();
	plane splatV = accumU.copy();

	plane_buffer accumUb = accumU;
	plane_buffer accumVb = accumV;
	plane_buffer splatb = splatV;
	const_plane_buffer inU = v.u();
	const_plane_buffer inV = v.v();
//	threading::get().dispatch( std::bind( project_splat_bilinear, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( accumUb ), std::ref( accumVb ), std::ref( splatV ), std::cref( inU ), std::cref( inV ), scale ), v.u() );
	project_splat_bilinear( 0, inU.y1(), inU.y2() + 1, accumUb, accumVb, splatb, inU, inV, scale );

	threading::get().dispatch( std::bind( project_final, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::ref( accumUb ), std::ref( accumVb ), std::cref( splatb ) ), v.u() );

	return vector_field( accumU, accumV, false );
}

} // empty namespace

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

vector_field
project( const vector_field &a, float scale )
{
	return vector_field( false, "v.project", a.dims(), convert_to_relative( a ), scale );
}

////////////////////////////////////////

plane confidence( const vector_field &a, const vector_field &b, int conservativeness )
{
	precondition( a.dims() == b.dims(), "Vector field not same size as other requested for divergence" );
	if ( conservativeness <= 0 )
		return log1p( magnitude( a.u() + warp_bilinear( b.u(), a ), a.v() + warp_bilinear( b.v(), a ) ) );

	return log1p( magnitude( a.u() + warp_bilinear( b.u(), a ), a.v() + warp_bilinear( b.v(), a ) ) *
				  magnitude( erode( a.u(), conservativeness ) - dilate( a.u(), conservativeness ),
							 erode( a.v(), conservativeness ) - dilate( a.v(), conservativeness ) ) );
}

////////////////////////////////////////

plane warp_dirac( const plane &src, const vector_field &v )
{
	precondition( v.x1() == src.x1() && v.y1() == src.y1() && v.x2() == src.x2() && v.y2() == src.y2(), "Vector field not same size as plane requested for warp" );
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
	precondition( v.x1() == src.x1() && v.y1() == src.y1() && v.x2() == src.x2() && v.y2() == src.y2(), "Vector field not same size as plane requested for warp" );
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
		return v;

	return modify_vectors(
		plane( "v.cvt_to_abs_u", v.u().dims(), v.u() ),
		plane( "v.cvt_to_abs_v", v.v().dims(), v.v() ),
		true );
}

////////////////////////////////////////

vector_field convert_to_relative( const vector_field &v )
{
	if ( v.is_absolute() )
		return modify_vectors(
			plane( "v.cvt_to_rel_u", v.u().dims(), v.u() ),
			plane( "v.cvt_to_rel_v", v.v().dims(), v.v() ),
			false );

	return v;
}

////////////////////////////////////////

image_buf
colorize( const vector_field &v, const plane &alpha, float scale )
{
	engine::dimensions d = v.dims();
	d.planes = 4;
	d.images = 1;
	if ( alpha.valid() )
	{
		precondition( v.u().dims() == alpha.dims(), "colorize must have an alpha channel of same size as vectors, received a {0} alpha {1}", v.dims(), alpha.dims() );
		return image_buf( "v.colorize_alpha", d, v, alpha, scale );
	}

	return image_buf( "v.colorize", d, v, scale );
}

////////////////////////////////////////

vector_field
modify_vectors( const plane &newu, const plane &newv, bool a )
{
	precondition( newu.dims() == newv.dims(), "modify planes must have vector u,v of same size, received u {0} v {1}", newu.dims(), newv.dims() );
	engine::dimensions d = newu.dims();
	d.planes = 2;

	return vector_field( a, "v.modify", d, newu, newv, a );
}

////////////////////////////////////////

vector_field concatenate( const vector_field &a, const vector_field &b )
{
	vector_field pull = b;
	if ( a.is_absolute() != b.is_absolute() )
	{
		if ( a.is_absolute() )
			pull = convert_to_absolute( b );
		else
			pull = convert_to_relative( b );
	}

	plane u = warp_bilinear( b.u(), a );
	plane v = warp_bilinear( b.u(), a );
	return vector_field( a.u() + u, a.v() + v, a.is_absolute() );
}

////////////////////////////////////////

void add_vector_ops( engine::registry &r )
{
	using namespace engine;

	r.register_constant<image::vector_field>();
	r.add( op( "v.extract_u", extract_u, op::simple ) );
	r.add( op( "v.extract_v", extract_v, op::simple ) );
	r.add( op( "v.modify", modify_uv, op::simple ) );

	r.add( op( "v.p.warp_dirac", base::choose_runtime( applyWarpDiracP ), n_scanline_plane_adapter<false, decltype(applyWarpDiracP)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "v.p.warp_bilinear", base::choose_runtime( applyWarpBilinearP ), n_scanline_plane_adapter<false, decltype(applyWarpBilinearP)>(), dispatch_scan_processing, op::n_to_one ) );

	r.add( op( "v.cvt_to_abs_u", base::choose_runtime( cvtToAbsU ), scanline_plane_adapter<true, decltype(cvtToAbsU)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "v.cvt_to_abs_v", base::choose_runtime( cvtToAbsV ), n_scanline_plane_adapter<false, decltype(cvtToAbsV)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "v.cvt_to_rel_u", base::choose_runtime( cvtToRelU ), scanline_plane_adapter<true, decltype(cvtToRelU)>(), dispatch_scan_processing, op::one_to_one ) );
	r.add( op( "v.cvt_to_rel_v", base::choose_runtime( cvtToRelV ), n_scanline_plane_adapter<false, decltype(cvtToRelV)>(), dispatch_scan_processing, op::n_to_one ) );

	r.add( op( "v.colorize", colorize_vector, op::threaded ) );
	r.add( op( "v.colorize_alpha", colorize_vector_alpha, op::threaded ) );

	r.add( op( "v.project", project_vector, op::threaded ) );
	
	add_oflow( r );
	add_patchmatch( r );
}

////////////////////////////////////////

} // namespace image

