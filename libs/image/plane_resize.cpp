// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

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
    int srcY = static_cast<int>(
                   ( static_cast<float>( y - in.y1() ) ) * scale + 0.5F ) +
               in.y1();
    srcY                = std::min( in.y2(), srcY );
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
        int srcX =
            static_cast<int>( ( static_cast<float>( x ) ) * scale + 0.5F );
        srcX    = std::min( maxS, srcX );
        dest[x] = in[srcX];
    }
}

////////////////////////////////////////

static void
doResizeVertBilinear( scanline &dest, int y, const plane &in, float scale )
{
    int   zeroY = y - in.y1();
    float srcY1 = ( static_cast<float>( zeroY ) + 0.5F ) * scale;
    int   p1    = static_cast<int>( srcY1 );
    float perc  = srcY1 - static_cast<float>( p1 );
    p1          = std::min( in.y2(), p1 + in.y1() );
    int p2      = std::min( in.y2(), p1 + 1 );

    const float *lowLine = in.line( p1 );
    const float *hiLine  = in.line( p2 );
    for ( int x = 0; x < dest.width(); ++x )
        dest[x] = base::lerp( lowLine[x], hiLine[x], perc );
}

static void
doResizeHorizBilinear( scanline &dest, const scanline &in, float scale )
{
    int maxw = in.width() - 1;
    for ( int x = 0; x < dest.width(); ++x )
    {
        float srcX1 = ( static_cast<float>( x ) + 0.5F ) * scale;
        int   p1    = static_cast<int>( srcX1 );
        float perc  = srcX1 - static_cast<float>( p1 );
        p1          = std::min( maxw, p1 );
        int p2      = std::min( maxw, p1 + 1 );
        dest[x]     = base::lerp( in[p1], in[p2], perc );
    }
}

////////////////////////////////////////

static void
doResizeVertBicubic( scanline &dest, int y, const plane &in, float scale )
{
    float srcY = static_cast<float>( y - in.y1() ) * scale +
                 static_cast<float>( in.y1() );
    int   pY = static_cast<int>( srcY );
    float t  = srcY - static_cast<float>( pY );

    pY              = std::min( in.y2(), pY );
    const float *p0 = in.line( std::max( in.y1(), pY - 1 ) );
    const float *p1 = in.line( pY );
    const float *p2 = in.line( std::min( in.y2(), pY + 1 ) );
    const float *p3 = in.line( std::min( in.y2(), pY + 2 ) );

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
        int   pX   = static_cast<int>( srcX );
        float t    = srcX - static_cast<float>( pX );
        pX         = std::min( wm1, pX );
        float p0   = in[std::max( int( 0 ), pX - 1 )];
        float p1   = in[pX];
        float p2   = in[std::min( int( wm1 ), pX + 1 )];
        float p3   = in[std::min( int( wm1 ), pX + 2 )];

        dest[x] = base::cubic_interp( t, p0, p1, p2, p3 );
    }
}

} // namespace

////////////////////////////////////////

namespace image
{
////////////////////////////////////////

plane resize_horiz_point( const plane &p, int neww )
{
    precondition( neww > 0, "Invalid new width {0} to resize", neww );
    engine::dimensions d = p.dims();
    d.x2                 = d.x1 + neww - 1;
    float scale = static_cast<float>( p.width() ) / static_cast<float>( neww );
    return plane( "p.resize_horiz_point", d, p, scale );
}

////////////////////////////////////////

plane resize_vert_point( const plane &p, int newh )
{
    precondition( newh > 0, "Invalid new width {0} to resize", newh );
    engine::dimensions d = p.dims();
    d.y2                 = d.y1 + newh - 1;
    float scale = static_cast<float>( p.height() ) / static_cast<float>( newh );
    return plane( "p.resize_vert_point", d, p, scale );
}

////////////////////////////////////////

plane resize_horiz_bilinear( const plane &p, int neww )
{
    precondition( neww > 0, "Invalid new width {0} to resize", neww );
    engine::dimensions d = p.dims();
    d.x2                 = d.x1 + neww - 1;
    float scale = static_cast<float>( p.width() ) / static_cast<float>( neww );
    return plane( "p.resize_horiz_bilinear", d, p, scale );
}

////////////////////////////////////////

plane resize_vert_bilinear( const plane &p, int newh )
{
    precondition( newh > 0, "Invalid new width {0} to resize", newh );
    engine::dimensions d = p.dims();
    d.y2                 = d.y1 + newh - 1;
    float scale = static_cast<float>( p.height() ) / static_cast<float>( newh );
    return plane( "p.resize_vert_bilinear", d, p, scale );
}

////////////////////////////////////////

plane resize_horiz_bicubic( const plane &p, int neww )
{
    precondition( neww > 0, "Invalid new width {0} to resize", neww );
    engine::dimensions d = p.dims();
    d.x2                 = d.x1 + neww - 1;
    float scale = static_cast<float>( p.width() ) / static_cast<float>( neww );
    return plane( "p.resize_horiz_bicubic", d, p, scale );
}

////////////////////////////////////////

plane resize_vert_bicubic( const plane &p, int newh )
{
    precondition( newh > 0, "Invalid new width {0} to resize", newh );
    engine::dimensions d = p.dims();
    d.y2                 = d.y1 + newh - 1;
    float scale = static_cast<float>( p.height() ) / static_cast<float>( newh );
    return plane( "p.resize_vert_bicubic", d, p, scale );
}

////////////////////////////////////////

plane resize_horiz( const plane &p, const std::string &filter, int neww )
{
    throw_not_yet();
}

////////////////////////////////////////

plane resize_vert( const plane &p, const std::string &filter, int newh )
{
    throw_not_yet();
}

////////////////////////////////////////

std::vector<plane> make_pyramid(
    const plane &in, const std::string &filter, float eta, int n, int minSize )
{
    precondition(
        eta > 0.F && eta < 1.F,
        "Invalid eta {0} for pyramid, must be between 0 and 1",
        eta );

    std::vector<plane> ret;
    int                curLev = 1;
    ret.push_back( in );
    while ( n == 0 || curLev < n )
    {
        float curS  = powf( eta, curLev );
        float curWf = curS * static_cast<float>( in.width() );
        float curHf = curS * static_cast<float>( in.height() );
        int   curW  = static_cast<int>( ceilf( curWf ) );
        int   curH  = static_cast<int>( ceilf( curHf ) );

        if ( curW < minSize || curH < minSize )
        {
            //			std::cout << "Terminating pyramid at level " << curLev << std::endl;
            break;
        }

        if ( curW >= ret.back().width() )
            curW = ret.back().width() - 1;
        if ( curH >= ret.back().height() )
            curH = ret.back().height() - 1;

//		std::cout << "adding level " << curLev << " at " << curW << "x" << curH << std::endl;
#define DIRECT_PYRAMIDS 1
#if DIRECT_PYRAMIDS
        plane tmp = in;
#else
        plane     tmp = ret.back();
#endif
        if ( filter == "bilinear" )
            ret.push_back( resize_bilinear( tmp, curW, curH ) );
        else if ( filter == "bicubic" )
            ret.push_back( resize_bicubic( tmp, curW, curH ) );
        else if ( filter == "point" || filter == "dirac" )
            ret.push_back( resize_point( tmp, curW, curH ) );
        else
            throw_not_yet();
        ++curLev;
    }

    return ret;
}

////////////////////////////////////////

std::vector<image_buf> make_pyramid(
    const image_buf &  in,
    const std::string &filter,
    float              eta,
    int                n,
    int                minSize )
{
    precondition(
        eta > 0.F && eta < 1.F,
        "Invalid eta {0} for pyramid, must be between 0 and 1",
        eta );

    std::vector<image_buf> ret;
    int                    curLev = 1;
    ret.push_back( in );
    while ( n == 0 || curLev < n )
    {
        float curS  = powf( eta, curLev );
        float curWf = curS * static_cast<float>( in.width() );
        float curHf = curS * static_cast<float>( in.height() );
        int   curW  = static_cast<int>( ceilf( curWf ) );
        int   curH  = static_cast<int>( ceilf( curHf ) );

        if ( curW < minSize || curH < minSize )
        {
            //			std::cout << "Terminating pyramid at level " << curLev << std::endl;
            break;
        }

        if ( curW >= ret.back().width() )
            curW = ret.back().width() - 1;
        if ( curH >= ret.back().height() )
            curH = ret.back().height() - 1;

//		std::cout << "adding level " << curLev << " at " << curW << "x" << curH << std::endl;
#if DIRECT_PYRAMIDS
        image_buf tmp = in;
#else
        image_buf tmp = ret.back();
#endif
        if ( filter == "bilinear" )
        {
            for ( size_t p = 0; p != in.size(); ++p )
                tmp[p] = resize_bilinear( in[p], curW, curH );
            ret.emplace_back( std::move( tmp ) );
        }
        else if ( filter == "bicubic" )
        {
            for ( size_t p = 0; p != tmp.size(); ++p )
                tmp[p] = resize_bicubic( tmp[p], curW, curH );
            ret.emplace_back( std::move( tmp ) );
        }
        else if ( filter == "point" || filter == "dirac" )
        {
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

void add_resize( engine::registry &r )
{
    using namespace engine;
    r.add(
        op( "p.resize_vert_point",
            base::choose_runtime( doResizeVertPoint ),
            n_scanline_plane_adapter<false, decltype( doResizeVertPoint )>(),
            dispatch_scan_processing,
            op::n_to_one ) );
    r.add(
        op( "p.resize_horiz_point",
            base::choose_runtime( doResizeHorizPoint ),
            scanline_plane_adapter<true, decltype( doResizeHorizPoint )>(),
            dispatch_scan_processing,
            op::one_to_one ) );

    r.add(
        op( "p.resize_vert_bilinear",
            base::choose_runtime( doResizeVertBilinear ),
            n_scanline_plane_adapter<false, decltype( doResizeVertBilinear )>(),
            dispatch_scan_processing,
            op::n_to_one ) );
    r.add(
        op( "p.resize_horiz_bilinear",
            base::choose_runtime( doResizeHorizBilinear ),
            scanline_plane_adapter<true, decltype( doResizeHorizBilinear )>(),
            dispatch_scan_processing,
            op::one_to_one ) );

    r.add(
        op( "p.resize_vert_bicubic",
            base::choose_runtime( doResizeVertBicubic ),
            n_scanline_plane_adapter<false, decltype( doResizeVertBicubic )>(),
            dispatch_scan_processing,
            op::n_to_one ) );
    r.add(
        op( "p.resize_horiz_bicubic",
            base::choose_runtime( doResizeHorizBicubic ),
            scanline_plane_adapter<true, decltype( doResizeHorizBicubic )>(),
            dispatch_scan_processing,
            op::one_to_one ) );

    //	r.add( op( "p.resize_vert_generic", base::choose_runtime( doResizeVertGeneric ), n_scanline_plane_adapter<false, decltype(doResizeVertGeneric)>(), dispatch_scan_processing, op::n_to_one ) );
    //	r.add( op( "p.resize_horiz_generic", base::choose_runtime( doResizeHorizGeneric ), scanline_plane_adapter<true, decltype(doResizeHorizGeneric)>(), dispatch_scan_processing, op::one_to_one ) );
}

////////////////////////////////////////

} // namespace image
