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

#include "plane_resize.h"
#include "scanline_process.h"
#include <base/cpu_features.h>

////////////////////////////////////////

namespace
{
using namespace image;

static void
doResizeVertPoint( scanline &dest, int y, const plane &in, float scale )
{
	int srcY = static_cast<int>( ( static_cast<float>( y ) ) * scale + 0.5F );
	srcY = std::min( in.height() - 1, srcY );
	const float *inLine = in.line( srcY );
	for ( int x = 0; x < dest.width(); ++x )
		dest[x] = inLine[x];
}

static void
doResizeHorizPoint( scanline &dest, const scanline &in, float scale )
{
	int maxS = in.width() - 1;
	for ( int x = 0; x < dest.width(); ++x )
	{
		int srcX = static_cast<int>( ( static_cast<float>( x ) ) * scale + 0.5F );
		srcX = std::min( maxS, srcX );
		dest[x] = in[srcX];
	}
}

////////////////////////////////////////

static void
doResizeVertBilinear( scanline &dest, int y, const plane &in, float scale )
{
	float srcY = static_cast<float>( y ) * scale;
	int lowY = static_cast<int>( srcY );
	float perc = srcY - static_cast<float>( lowY );
	lowY = std::min( in.height() - 1, lowY );

	const float *lowLine = in.line( lowY );
	const float *hiLine = lowLine;
	if ( ( lowY + 1 ) < in.height() )
		hiLine = in.line( lowY + 1 );
	for ( int x = 0; x < dest.width(); ++x )
		dest[x] = base::lerp( lowLine[x], hiLine[x], perc );
}

static void
doResizeHorizBilinear( scanline &dest, const scanline &in, float scale )
{
	int maxw = in.width() - 1;
	for ( int x = 0; x < dest.width(); ++x )
	{
		float srcX = static_cast<float>( x ) * scale;
		int lowX = static_cast<int>( srcX );
		float perc = srcX - static_cast<float>( lowX );
		lowX = std::min( maxw, lowX );
		int hiX = std::min( maxw, lowX + 1 );

		dest[x] = base::lerp( in[lowX], in[hiX], perc );
	}
}

////////////////////////////////////////

static void
doResizeVertBicubic( scanline &dest, int y, const plane &in, float scale )
{
	float srcY = static_cast<float>( y ) * scale;
	int pY = static_cast<int>( srcY );
	float t = srcY - static_cast<float>( pY );
	int hm1 = in.height() - 1;
	pY = std::min( hm1, pY );
	const float *p0 = in.line( std::max( int(0), pY - 1 ) );
	const float *p1 = in.line( pY );
	const float *p2 = in.line( std::min( int(hm1), pY + 1 ) );
	const float *p3 = in.line( std::min( int(hm1), pY + 2 ) );

	for ( int x = 0; x < dest.width(); ++x )
		dest[x] = base::cubic_interp( t, p0[x], p1[x], p2[x], p3[x] );
}

static void
doResizeHorizBicubic( scanline &dest, const scanline &in, float scale )
{
	int wm1 = in.width() - 1;
	for ( int x = 0; x < dest.width(); ++x )
	{
		float srcX = static_cast<float>( x ) * scale;
		int pX = static_cast<int>( srcX );
		float t = srcX - static_cast<float>( pX );
		pX = std::min( wm1, pX );
		float p0 = in[std::max( int(0), pX - 1 )];
		float p1 = in[pX];
		float p2 = in[std::min( int(wm1), pX + 1 )];
		float p3 = in[std::min( int(wm1), pX + 2 )];

		dest[x] = base::cubic_interp( t, p0, p1, p2, p3 );
	}
}

} // empty namespace

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

plane
resize_horiz_point( const plane &p, int neww )
{
	precondition( neww > 0, "Invalid new width {0} to resize", neww );
	engine::dimensions d = p.dims();
	d.x = neww;
	float scale = static_cast<float>( p.width() ) / static_cast<float>( neww );
	return plane( "p.resize_horiz_point", d, p, scale );
}

////////////////////////////////////////

plane
resize_vert_point( const plane &p, int newh )
{
	precondition( newh > 0, "Invalid new width {0} to resize", newh );
	engine::dimensions d = p.dims();
	d.y = newh;
	float scale = static_cast<float>( p.height() ) / static_cast<float>( newh );
	return plane( "p.resize_vert_point", d, p, scale );
}

////////////////////////////////////////

plane
resize_horiz_bilinear( const plane &p, int neww )
{
	precondition( neww > 0, "Invalid new width {0} to resize", neww );
	engine::dimensions d = p.dims();
	d.x = neww;
	float scale = static_cast<float>( p.width() ) / static_cast<float>( neww );
	return plane( "p.resize_horiz_bilinear", d, p, scale );
}

////////////////////////////////////////

plane
resize_vert_bilinear( const plane &p, int newh )
{
	precondition( newh > 0, "Invalid new width {0} to resize", newh );
	engine::dimensions d = p.dims();
	d.y = newh;
	float scale = static_cast<float>( p.height() ) / static_cast<float>( newh );
	return plane( "p.resize_vert_bilinear", d, p, scale );
}

////////////////////////////////////////

plane
resize_horiz_bicubic( const plane &p, int neww )
{
	precondition( neww > 0, "Invalid new width {0} to resize", neww );
	engine::dimensions d = p.dims();
	d.x = neww;
	float scale = static_cast<float>( p.width() ) / static_cast<float>( neww );
	return plane( "p.resize_horiz_bicubic", d, p, scale );
}

////////////////////////////////////////

plane
resize_vert_bicubic( const plane &p, int newh )
{
	precondition( newh > 0, "Invalid new width {0} to resize", newh );
	engine::dimensions d = p.dims();
	d.y = newh;
	float scale = static_cast<float>( p.height() ) / static_cast<float>( newh );
	return plane( "p.resize_vert_bicubic", d, p, scale );
}

////////////////////////////////////////

plane
resize_horiz( const plane &p, const std::string &filter, int neww )
{
	throw_not_yet();
}

////////////////////////////////////////

plane
resize_vert( const plane &p, const std::string &filter, int newh )
{
	throw_not_yet();
}

////////////////////////////////////////

std::vector<plane>
make_pyramid( const plane &in, const std::string &filter, float eta, int n, int minSize )
{
	precondition( eta > 0.F && eta < 1.F, "Invalid eta {0} for pyramid, must be between 0 and 1", eta );

	std::vector<plane> ret;
	int curLev = 1;
	ret.push_back( in );
	while ( n == 0 || curLev < n )
	{
		float curS = powf( eta, curLev );
		float curWf = curS * static_cast<float>( in.width() );
		float curHf = curS * static_cast<float>( in.height() );
		int curW = static_cast<int>( ceilf( curWf ) );
		int curH = static_cast<int>( ceilf( curHf ) );

		if ( curW < minSize || curH < minSize )
		{
//			std::cout << "Terminating pyramid at level " << curLev << std::endl;
			break;
		}

//		std::cout << "adding level " << curLev << " at " << curW << "x" << curH << std::endl;
		if ( filter == "bilinear" )
			ret.push_back( resize_bilinear( ret.back(), curW, curH ) );
		else if ( filter == "bicubic" )
			ret.push_back( resize_bicubic( ret.back(), curW, curH ) );
		else if ( filter == "point" || filter == "dirac" )
			ret.push_back( resize_point( ret.back(), curW, curH ) );
		else
			throw_not_yet();

		++curLev;
	}

	return ret;
}

////////////////////////////////////////

std::vector<image_buf>
make_pyramid( const image_buf &in, const std::string &filter, float eta, int n, int minSize )
{
	precondition( eta > 0.F && eta < 1.F, "Invalid eta {0} for pyramid, must be between 0 and 1", eta );

	std::vector<image_buf> ret;
	int curLev = 1;
	ret.push_back( in );
	while ( n == 0 || curLev < n )
	{
		float curS = powf( eta, curLev );
		float curWf = curS * static_cast<float>( in.width() );
		float curHf = curS * static_cast<float>( in.height() );
		int curW = static_cast<int>( ceilf( curWf ) );
		int curH = static_cast<int>( ceilf( curHf ) );

		if ( curW < minSize || curH < minSize )
		{
//			std::cout << "Terminating pyramid at level " << curLev << std::endl;
			break;
		}

//		std::cout << "adding level " << curLev << " at " << curW << "x" << curH << std::endl;
		if ( filter == "bilinear" )
		{
			image_buf tmp = ret.back();
			for ( size_t p = 0; p != tmp.size(); ++p )
				tmp[p] = resize_bilinear( tmp[p], curW, curH );
			ret.emplace_back( std::move( tmp ) );
		}
		else if ( filter == "bicubic" )
		{
			image_buf tmp = ret.back();
			for ( size_t p = 0; p != tmp.size(); ++p )
				tmp[p] = resize_bicubic( tmp[p], curW, curH );
			ret.emplace_back( std::move( tmp ) );
		}
		else if ( filter == "point" || filter == "dirac" )
		{
			image_buf tmp = ret.back();
			for ( size_t p = 0; p != tmp.size(); ++p )
				tmp[p] = resize_point( tmp[p], curW, curH );
			ret.emplace_back( std::move( tmp ) );
		}
		else
		{
			throw_not_yet();
		}
		++curLev;
	}

	return ret;
}

////////////////////////////////////////

void
add_resize( engine::registry &r )
{
	using namespace engine;
	r.add( op( "p.resize_vert_point", base::choose_runtime( doResizeVertPoint ), n_scanline_plane_adapter<false, decltype(doResizeVertPoint)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "p.resize_horiz_point", base::choose_runtime( doResizeHorizPoint ), scanline_plane_adapter<true, decltype(doResizeHorizPoint)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "p.resize_vert_bilinear", base::choose_runtime( doResizeVertBilinear ), n_scanline_plane_adapter<false, decltype(doResizeVertBilinear)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "p.resize_horiz_bilinear", base::choose_runtime( doResizeHorizBilinear ), scanline_plane_adapter<true, decltype(doResizeHorizBilinear)>(), dispatch_scan_processing, op::one_to_one ) );

	r.add( op( "p.resize_vert_bicubic", base::choose_runtime( doResizeVertBicubic ), n_scanline_plane_adapter<false, decltype(doResizeVertBicubic)>(), dispatch_scan_processing, op::n_to_one ) );
	r.add( op( "p.resize_horiz_bicubic", base::choose_runtime( doResizeHorizBicubic ), scanline_plane_adapter<true, decltype(doResizeHorizBicubic)>(), dispatch_scan_processing, op::one_to_one ) );

//	r.add( op( "p.resize_vert_generic", base::choose_runtime( doResizeVertGeneric ), n_scanline_plane_adapter<false, decltype(doResizeVertGeneric)>(), dispatch_scan_processing, op::n_to_one ) );
//	r.add( op( "p.resize_horiz_generic", base::choose_runtime( doResizeHorizGeneric ), scanline_plane_adapter<true, decltype(doResizeHorizGeneric)>(), dispatch_scan_processing, op::one_to_one ) );
}

////////////////////////////////////////

} // image



