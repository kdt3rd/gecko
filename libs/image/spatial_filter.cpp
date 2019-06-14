// Copyright (c) 2016 Kimball Thurston
// SPDX-License-Identifier: MIT

#include "spatial_filter.h"

#include "plane_ops.h"
#include "scanline_process.h"
#include "threading.h"

#include <base/contract.h>
#include <base/cpu_features.h>
#include <base/math_functions.h>
#include <base/svd.h>

////////////////////////////////////////

namespace
{
using namespace image;

////////////////////////////////////////

void apply_erode( scanline &dest, int y, const plane &p, int radius )
{
    int maxx = p.width() - 1;

    for ( int x = 0, w = dest.width(); x < w; ++x )
    {
        float minV = dest[x];
        for ( int cy = y - radius; cy <= y + radius; ++cy )
        {
            if ( cy < p.y1() || cy > p.y2() )
                continue;

            const float *inP = p.line( cy );
            for ( int cx = x - radius; cx <= x + radius; ++cx )
            {
                if ( cx < 0 || cy >= maxx )
                    continue;

                minV = std::min( minV, inP[cx] );
            }
        }
        dest[x] = minV;
    }
}

////////////////////////////////////////

void apply_dilate( scanline &dest, int y, const plane &p, int radius )
{
    int maxx = p.width() - 1;

    for ( int x = 0, w = dest.width(); x < w; ++x )
    {
        float maxV = dest[x];
        for ( int cy = y - radius; cy <= y + radius; ++cy )
        {
            if ( cy < p.y1() || cy > p.y2() )
                continue;

            const float *inP = p.line( cy );
            for ( int cx = x - radius; cx <= x + radius; ++cx )
            {
                if ( cx < 0 || cy >= maxx )
                    continue;

                maxV = std::max( maxV, inP[cx] );
            }
        }
        dest[x] = maxV;
    }
}

////////////////////////////////////////

inline void sort( float &a, float &b )
{
    float t = fmaxf( a, b );
    a       = fminf( a, b );
    b       = t;
}
#define mn3( a, b, c )                                                         \
    sort( a, b );                                                              \
    sort( a, c );
#define mx3( a, b, c )                                                         \
    sort( b, c );                                                              \
    sort( a, c );
#define mnmx3( a, b, c )                                                       \
    mx3( a, b, c );                                                            \
    sort( a, b );
#define mnmx4( a, b, c, d )                                                    \
    sort( a, b );                                                              \
    sort( c, d );                                                              \
    sort( a, c );                                                              \
    sort( b, d );
#define mnmx5( a, b, c, d, e )                                                 \
    sort( a, b );                                                              \
    sort( c, d );                                                              \
    mn3( a, c, e );                                                            \
    mx3( b, d, e );
#define mnmx6( a, b, c, d, e, f )                                              \
    sort( a, d );                                                              \
    sort( b, e );                                                              \
    sort( c, f );                                                              \
    mn3( a, b, c );                                                            \
    mx3( d, e, f );
#define mnmx9( a, b, c, d, e, f, g, h, k )                                     \
    sort( a, d );                                                              \
    sort( b, e );                                                              \
    sort( c, f );                                                              \
    mn3( a, b, c );                                                            \
    mx3( d, e, f );                                                            \
    mnmx5( g, b, c, d, e );                                                    \
    mnmx4( h, b, c, d );                                                       \
    mnmx3( k, b, c );

////////////////////////////////////////

void median_3x3( scanline &dest, int y, const plane &p )
{
    int sy1  = std::min( p.y2(), y + 1 );
    int sym1 = std::max( p.y1(), y - 1 );

    scanline p1 = scan_ref( p, sy1 );
    scanline c1 = scan_ref( p, y );
    scanline m1 = scan_ref( p, sym1 );

    float r1, r2, r3, r4, r5, r6;
    for ( int x = 0, w = dest.width(); x < w; ++x )
    {
        int xm1 = std::max( int( 0 ), x - 1 );
        int xp1 = std::min( w - 1, x + 1 );

        r1 = m1[xm1];
        r2 = m1[x];
        r3 = m1[xp1];
        r4 = c1[xm1];
        r5 = c1[x];
        r6 = c1[xp1];
        mnmx6( r1, r2, r3, r4, r5, r6 );
        r1 = p1[xm1];
        mnmx5( r1, r2, r3, r4, r5 );
        r1 = p1[x];
        mnmx4( r1, r2, r3, r4 );
        r1 = p1[xp1];
        mnmx3( r1, r2, r3 );
        dest[x] = r2;
    }
}

////////////////////////////////////////

static void generic_median_thread(
    size_t, int s, int e, plane &r, const plane &p, int diam )
{
    std::vector<float> tmpV;
    tmpV.resize( static_cast<size_t>( diam * diam ) );

    int    halfD  = diam / 2;
    bool   even   = halfD * 2 == diam;
    int    w      = p.width();
    int    wm1    = w - 1;
    size_t middle = tmpV.size() / 2;
    for ( int y = s; y < e; ++y )
    {
        int fy = y - halfD;
        int ty = y + halfD;
        if ( even )
            ++fy;
        float *destP = r.line( y );
        for ( int x = 0; x < w; ++x )
        {
            size_t i = 0;

            int fx = x - halfD;
            int tx = x + halfD;
            if ( even )
                ++fx;
            for ( int cy = fy; cy <= ty; ++cy )
            {
                int          ready = std::min( r.y2(), std::max( r.y1(), cy ) );
                const float *lineP = p.line( ready );

                for ( int cx = fx; cx <= tx; ++cx )
                {
                    int readx = std::min( wm1, std::max( int( 0 ), cx ) );
                    tmpV[i++] = lineP[readx];
                }
            }
            std::partial_sort(
                tmpV.begin(),
                tmpV.begin() + static_cast<long>( middle + 1 ),
                tmpV.end() );
            destP[x] = tmpV[middle];
        }
    }
}

static plane generic_median( const plane &p, int diam )
{
    plane r( p.x1(), p.y1(), p.x2(), p.y2() );

    threading::get().dispatch(
        std::bind(
            generic_median_thread,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3,
            std::ref( r ),
            std::cref( p ),
            diam ),
        p );

    return r;
}

////////////////////////////////////////

void cross_x_median( scanline &dest, int y, const plane &p )
{
    scanline s = scan_ref( p, y );
    if ( dest.width() < 5 )
    {
        for ( int x = 0; x != dest.width(); ++x )
            dest[x] = s[x];
        return;
    }
    int syp2 = std::min( p.y2(), y + 1 );
    int syp1 = std::min( p.y2(), y + 1 );
    int sym1 = std::max( p.y1(), y - 1 );
    int sym2 = std::max( p.y1(), y - 2 );

    scanline m2 = scan_ref( p, sym2 );
    scanline m1 = scan_ref( p, sym1 );
    scanline p1 = scan_ref( p, syp1 );
    scanline p2 = scan_ref( p, syp2 );

    float x1, x2, x3, x4, x5, x6, x7, x8, x9;
    float c1, c2, c3, c4, c5, c6, c7, c8, c9;
    float img;
    int   wm1 = dest.width() - 1;
    for ( int x = 0; x <= wm1; ++x )
    {
        int xm2 = std::max( int( 0 ), x - 2 );
        int xm1 = std::max( int( 0 ), x - 1 );
        int xp1 = std::min( wm1, x + 1 );
        int xp2 = std::min( wm1, x + 2 );
        x1      = m2[xm2];
        c1      = m2[x];
        x2      = m2[xp2];
        x3      = m1[xm1];
        c2      = m1[x];
        x4      = m1[xp1];
        c3      = s[xm2];
        c4      = s[xm1];
        c5      = s[x];
        x5      = c5;
        img     = c5;
        c6      = s[xp1];
        c7      = s[xp2];
        x6      = p1[xm1];
        c8      = p1[x];
        x7      = p1[xp1];
        x8      = p2[xm2];
        c9      = p2[x];
        x9      = p2[xp2];
        mnmx9( c1, c2, c3, c4, c5, c6, c7, c8, c9 );
        mnmx9( x1, x2, x3, x4, x5, x6, x7, x8, x9 );
        mnmx3( img, c2, x2 );
        dest[x] = c2;
    }
}

////////////////////////////////////////

void doDespeckle(
    scanline &dest, int y, const plane &p, float bright, float dark )
{
    if ( y == p.y1() || y == p.y2() )
    {
        scanline c1 = scan_ref( p, y );
        for ( int x = 0, w = dest.width(); x < w; ++x )
            dest[x] = c1[x];
        return;
    }

    scanline p1 = scan_ref( p, y + 1 );
    scanline c1 = scan_ref( p, y );
    scanline m1 = scan_ref( p, y - 1 );

    float r1, r2, r3, r4, r5, r6, r7, r8, r9;
    dest[0] = c1[0];
    for ( int x = 1, w = dest.width() - 1, xm1 = 0, xp1 = 2; x < w;
          ++x, ++xm1, ++xp1 )
    {
        r1        = m1[xm1];
        r2        = m1[x];
        r3        = m1[xp1];
        r4        = c1[xm1];
        r5        = c1[x];
        r6        = c1[xp1];
        r7        = p1[xm1];
        r8        = p1[x];
        r9        = p1[xp1];
        float gl  = r5 - r4;
        float gr  = r5 - r6;
        float gul = r5 - r1;
        float gum = r5 - r2;
        float gur = r5 - r3;
        float gll = r5 - r7;
        float glm = r5 - r8;
        float glr = r5 - r9;
        if ( gl > 0.F && gr > 0.F && gul > 0.F && gum > 0.F && gur > 0.F &&
             gll > 0.F && glm > 0.F && glr > 0.F )
        {
            float orig = r5;
            mnmx9( r1, r2, r3, r4, r5, r6, r7, r8, r9 );
            dest[x] = base::lerp( orig, r2, bright );
        }
        else if (
            gl < 0.F && gr < 0.F && gul < 0.F && gum < 0.F && gur < 0.F &&
            gll < 0.F && glm < 0.F && glr < 0.F )
        {
            float orig = r5;
            mnmx9( r1, r2, r3, r4, r5, r6, r7, r8, r9 );
            dest[x] = base::lerp( orig, r2, dark );
        }
        else
        {
            dest[x] = r5;
        }
    }
    dest[dest.width() - 1] = c1[dest.width() - 1];
}

////////////////////////////////////////

void median_planes(
    scanline &dest, const scanline &a, const scanline &b, const scanline &c )
{
    for ( int x = 0, w = dest.width(); x < w; ++x )
    {
        float av = a[x];
        float bv = b[x];
        float cv = c[x];
        sort( av, bv );
        sort( bv, cv );
        sort( av, bv );
        dest[x] = bv;
    }
}

////////////////////////////////////////

static void cross_bilateral_thread(
    size_t,
    int          s,
    int          e,
    plane &      r,
    const plane &p,
    const plane &ref,
    int          dx,
    int          dy,
    float        sigR,
    float        sigI )
{
    std::vector<float> sumW;
    sumW.resize( static_cast<size_t>( p.width() ), 0.F );

    int   w    = p.width();
    int   wm1  = w - 1;
    float dsig = -1.F / ( sigR * sigR * 2.F );
    float isig = -1.F / ( sigI * sigI * 2.F );
    for ( int y = s; y < e; ++y )
    {
        float *destP = r.line( y );
        for ( int x = 0; x < w; ++x )
            destP[x] = 0.F;

        const float *cenP = ref.line( y );
        for ( int cy = y - dy; cy <= y + dy; ++cy )
        {
            int          rY    = std::max( p.y1(), std::min( p.y2(), cy ) );
            int          distY = ( cy - y ) * ( cy - y );
            const float *srcP  = p.line( rY );
            const float *refP  = ref.line( rY );
            for ( int x = 0; x < w; ++x )
            {
                float cenV = cenP[x];
                for ( int cx = x - dx; cx <= x + dx; ++cx )
                {
                    int   rX     = std::max( int( 0 ), std::min( wm1, cx ) );
                    int   distX  = ( cx - x ) * ( cx - x );
                    float oV     = refP[rX];
                    float weight = expf(
                        static_cast<float>( distY + distX ) * dsig +
                        ( ( oV - cenV ) * ( oV - cenV ) ) * isig );
                    sumW[static_cast<size_t>( x )] += weight;
                    destP[x] += weight * srcP[rX];
                }
            }
        }

        for ( int x = 0; x < w; ++x )
        {
            float &weight = sumW[static_cast<size_t>( x )];
            destP[x] /= weight;
            weight = 0.F;
        }
    }
}

static plane
apply_bilateral( const plane &p, int dx, int dy, float sigR, float sigI )
{
    plane r( p.x1(), p.y1(), p.x2(), p.y2() );

    threading::get().dispatch(
        std::bind(
            cross_bilateral_thread,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3,
            std::ref( r ),
            std::cref( p ),
            std::cref( p ),
            dx,
            dy,
            sigR,
            sigI ),
        p );

    return r;
}

static plane apply_cross_bilateral(
    const plane &p, const plane &ref, int dx, int dy, float sigR, float sigI )
{
    plane r( p.x1(), p.y1(), p.x2(), p.y2() );

    threading::get().dispatch(
        std::bind(
            cross_bilateral_thread,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3,
            std::ref( r ),
            std::cref( p ),
            std::cref( ref ),
            dx,
            dy,
            sigR,
            sigI ),
        p );

    return r;
}

////////////////////////////////////////

static void weighted_bilateral_thread(
    size_t,
    int          s,
    int          e,
    plane &      r,
    const plane &p,
    const plane &weight,
    int          dx,
    int          dy,
    float        sigR,
    float        sigI )
{
    std::vector<float> sumW;
    sumW.resize( static_cast<size_t>( p.width() ), 0.F );

    int   w    = p.width();
    int   wm1  = w - 1;
    float dsig = -1.F / ( sigR * sigR * 2.F );
    for ( int y = s; y < e; ++y )
    {
        float *destP = r.line( y );
        for ( int x = 0; x < w; ++x )
            destP[x] = 0.F;

        const float *weightP = weight.line( y );
        const float *cenP    = p.line( y );
        for ( int cy = y - dy; cy <= y + dy; ++cy )
        {
            int          rY    = std::max( p.y1(), std::min( p.y2(), cy ) );
            int          distY = ( cy - y ) * ( cy - y );
            const float *srcP  = p.line( rY );
            for ( int x = 0; x < w; ++x )
            {
                float cenV = cenP[x];
                float tsig = sigI * weightP[x];
                float isig = -1.F / ( tsig * tsig * 2.F );
                for ( int cx = x - dx; cx <= x + dx; ++cx )
                {
                    int   rX      = std::max( int( 0 ), std::min( wm1, cx ) );
                    int   distX   = ( cx - x ) * ( cx - x );
                    float oV      = srcP[rX];
                    float weightV = expf(
                        static_cast<float>( distY + distX ) * dsig +
                        ( ( oV - cenV ) * ( oV - cenV ) ) * isig );
                    sumW[static_cast<size_t>( x )] += weightV;
                    destP[x] += weightV * srcP[rX];
                }
            }
        }

        for ( int x = 0; x < w; ++x )
        {
            float &weightV = sumW[static_cast<size_t>( x )];
            destP[x] /= weightV;
            weightV = 0.F;
        }
    }
}

static plane apply_weighted_bilateral(
    const plane &p, const plane &w, int dx, int dy, float sigR, float sigI )
{
    plane r( p.x1(), p.y1(), p.x2(), p.y2() );

    threading::get().dispatch(
        std::bind(
            weighted_bilateral_thread,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3,
            std::ref( r ),
            std::cref( p ),
            std::cref( w ),
            dx,
            dy,
            sigR,
            sigI ),
        p );

    return r;
}

////////////////////////////////////////

static inline std::tuple<plane, plane, plane, plane> wavelet_decomp(
    const plane &p, const std::vector<float> &h, const std::vector<float> &g )
{
    plane hhc   = convolve_vert( p, h );
    plane ghc   = convolve_vert( p, g );
    plane c_j1  = convolve_horiz( hhc, h );
    plane w1_j1 = convolve_horiz( hhc, g );
    plane w2_j1 = convolve_horiz( ghc, h );
    plane w3_j1 = convolve_horiz( ghc, g );

    return std::make_tuple( c_j1, w1_j1, w2_j1, w3_j1 );
}

//inline plane
//logistic( const plane &x, float x0, float k = 1.F, float L = 1.F )
//{
//	return L / ( 1.F + exp( (-k) * ( x - x0 ) ) );
//}

inline plane gaussian( const plane &x, float a, float b, float c )
{
    return a * exp( abs( x - b ) / ( -2.F * c * c ) );
}

inline plane gaussian( const plane &x, float a, float b, const plane &sigma )
{
    return a * exp( abs( x - b ) / ( -2.F * square( sigma ) ) );
}

template <typename Sigma>
plane wavelet_filter_impl( const plane &p, int levels, Sigma sigma )
{
    precondition( levels > 0, "invalid levels {0}", levels );
    std::vector<std::tuple<plane, plane, plane>> filtLevels;

    std::vector<float> wt_h{
        1.F / 16.F, 4.F / 16.F, 6.F / 16.F, 4.F / 16.F, 1.F / 16.F
    };
    std::vector<float> wt_g = base::dirac_negate( wt_h );

    plane c_J = p;
    int   cnt = levels;
    while ( true )
    {
        auto wd = wavelet_decomp( c_J, wt_h, wt_g );
        c_J     = std::get<0>( wd );
        filtLevels.push_back( std::make_tuple(
            std::get<1>( wd ), std::get<2>( wd ), std::get<3>( wd ) ) );
        if ( cnt == 0 )
            break;

        --cnt;
        wt_h = base::atrous_expand( wt_h );
        wt_g = base::atrous_expand( wt_g );
    }

    postcondition(
        filtLevels.size() == ( levels + 1 ),
        "Expecting {0} levels",
        ( levels + 1 ) );

    float levelScale = 6.F / 16.F;
    for ( int l = 0; l < levels; ++l )
    {
        auto &curL = filtLevels[l];
        // todo: add next level correlation
        //		auto &nextL = filtLevels[l+1];
        plane &curPh = std::get<0>( curL );
        plane &curPv = std::get<1>( curL );
        plane &curPc = std::get<2>( curL );

        plane s1 = ( 1.F - gaussian( curPh, 1.F, 0.F, sigma ) );
        plane s2 = ( 1.F - gaussian( curPv, 1.F, 0.F, sigma ) );
        plane s3 = ( 1.F - gaussian( curPc, 1.F, 0.F, sigma ) );

        plane w = max( s1, max( s2, s3 ) );
        curPh   = curPh * w;
        curPv   = curPv * w;
        curPc   = curPc * w;

        sigma *= levelScale;
    }

    plane reconst = c_J;
    for ( auto &l: filtLevels )
        reconst += std::get<0>( l ) + std::get<1>( l ) + std::get<2>( l );

    return reconst;
}

template <typename Epsilon>
inline plane
guided_filter_impl( const plane &I, const plane &p, int r, Epsilon eps )
{
    precondition(
        p.dims() == I.dims(),
        "unable to guided_filter planes of different sizes" );
    plane mean_I = local_mean( I, r );
    //	plane mean_II = local_mean( square( I ), r );
    //	plane var_I = mean_II - square( mean_I );
    plane var_I = local_variance( I, r );

    plane mean_p  = local_mean( p, r );
    plane mean_Ip = local_mean( I * p, r );
    plane cov_Ip  = mean_Ip - mean_I * mean_p;

    plane a = cov_Ip / ( var_I + eps );
    plane b = mean_p - a * mean_I;

    plane mean_a = local_mean( a, r );
    plane mean_b = local_mean( b, r );
    return mean_a * I + mean_b;
}

template <typename Epsilon>
inline image_buf guided_filter_color_impl(
    const image_buf &I, const image_buf &p, int r, Epsilon eps )
{
    image_buf ret = p;
    if ( I.size() >= 3 && p.size() >= 3 )
    {
        plane mean_I_r = local_mean( I[0], r );
        plane mean_I_g = local_mean( I[1], r );
        plane mean_I_b = local_mean( I[2], r );

        // variance becomes a matrix
        // [ rr rg rb
        //   rg gg gb
        //   rb gb bb ]
        plane var_I_rr =
            local_mean( I[0] * I[0], r ) - mean_I_r * mean_I_r + eps;
        plane var_I_rg = local_mean( I[0] * I[1], r ) - mean_I_r * mean_I_g;
        plane var_I_rb = local_mean( I[0] * I[2], r ) - mean_I_r * mean_I_b;
        plane var_I_gg =
            local_mean( I[1] * I[1], r ) - mean_I_g * mean_I_g + eps;
        plane var_I_gb = local_mean( I[1] * I[2], r ) - mean_I_g * mean_I_b;
        plane var_I_bb =
            local_mean( I[2] * I[2], r ) - mean_I_b * mean_I_b + eps;

        plane invrr = var_I_gg * var_I_bb - var_I_gb * var_I_gb;
        plane invrg = var_I_gb * var_I_rb - var_I_rg * var_I_bb;
        plane invrb = var_I_rg * var_I_gb - var_I_gg * var_I_rb;
        plane invgg = var_I_rr * var_I_bb - var_I_rb * var_I_rb;
        plane invgb = var_I_rb * var_I_rg - var_I_rr * var_I_gb;
        plane invbb = var_I_rr * var_I_gg - var_I_rg * var_I_rg;

        plane det = invrr * var_I_rr + invrg * var_I_rg + invrb * var_I_rb;
        invrr /= det;
        invrg /= det;
        invrb /= det;
        invgg /= det;
        invgb /= det;
        invbb /= det;

        for ( size_t i = 0, N = ret.size(); i != N; ++i )
        {
            plane mean_p    = local_mean( p[i], r );
            plane mean_Ip_r = local_mean( I[0] * p[i], r );
            plane mean_Ip_g = local_mean( I[1] * p[i], r );
            plane mean_Ip_b = local_mean( I[2] * p[i], r );
            plane cov_Ip_r  = mean_Ip_r - mean_I_r * mean_p;
            plane cov_Ip_g  = mean_Ip_g - mean_I_g * mean_p;
            plane cov_Ip_b  = mean_Ip_b - mean_I_b * mean_p;
            plane a_r = invrr * cov_Ip_r + invrg * cov_Ip_g + invrb * cov_Ip_b;
            plane a_g = invrg * cov_Ip_r + invgg * cov_Ip_g + invgb * cov_Ip_b;
            plane a_b = invrb * cov_Ip_r + invgb * cov_Ip_g + invbb * cov_Ip_b;
            plane b = mean_p - a_r * mean_I_r - a_g * mean_I_g - a_b * mean_I_b;

            ret[i] = local_mean( a_r, r ) * I[0] + local_mean( a_g, r ) * I[1] +
                     local_mean( a_b, r ) * I[2] + local_mean( b, r );
        }
    }
    else
    {
        precondition( I.size() >= p.size(), "mismatch in plane count" );
        for ( size_t i = 0, N = ret.size(); i != N; ++i )
            ret[i] = guided_filter_mono( I[i], p[i], r, eps );
    }

    return ret;
}

static void sav_gol_thread(
    size_t,
    int                                s,
    int                                e,
    plane &                            r,
    const plane &                      p,
    const math::svd<float>::grid_type &A,
    int                                radius )
{
    const size_t M = A.size();

    int w    = r.width();
    int offX = r.x1();
    for ( int y = s; y < e; ++y )
    {
        float *      destP = r.line( y );
        const float *srcP  = p.line( y );
        for ( int x = 0; x < w; ++x )
        {
            float rV = srcP[x];

            int    curX  = -radius;
            int    curY  = curX;
            double sum2R = 0.0;
            for ( auto uv: A[0] )
                sum2R += static_cast<double>( rV ) * static_cast<double>( uv );

            for ( size_t row = 1; row != M; ++row )
            {
                if ( 0 == curY && 0 == curX )
                    ++curX;

                rV = get_mirror( p, offX + x + curX, y + curY );
                for ( auto uv: A[row] )
                    sum2R +=
                        static_cast<double>( rV ) * static_cast<double>( uv );

                ++curX;
                if ( curX > radius )
                {
                    curX = -radius;
                    ++curY;
                }
            }
            destP[x] = static_cast<float>( sum2R );
        }
    }
}

static plane apply_sav_gol( const plane &p, int radius, int order )
{
    plane r( p.x1(), p.y1(), p.x2(), p.y2() );

    typedef math::svd<float> svdf;
    svdf                     savFunc;
    svdf::grid_type          A;
    const size_t             W = radius * 2 + 1;
    const size_t             M = W * W;
    const size_t             N = ( order + 1 ) * ( order + 2 ) / 2;

    A.resize( M );
    int   curX     = -radius;
    int   curY     = -radius;
    float posScale = 1.F / float( radius );
    A[0].resize( N, 0.F );
    A[0][0] = 1.F;
    for ( size_t row = 1; row != M; ++row )
    {
        if ( curY == 0 && curX == 0 )
            ++curX;

        svdf::col_type &coeff = A[row];
        coeff.resize( N, 0.F );
        coeff[0]      = 1.F;
        size_t curIdx = 1;
        for ( int i = 1; i <= order; ++i )
        {
            int xPow = i, yPow = 0;
            while ( xPow >= 0 )
            {
                float xV      = powf( float( curX ) * posScale, xPow );
                float yV      = powf( float( curY ) * posScale, yPow );
                coeff[curIdx] = xV * yV;
                --xPow;
                ++yPow;
                ++curIdx;
            }
        }

        ++curX;
        if ( curX > radius )
        {
            curX = -radius;
            ++curY;
        }
    }

    //	std::cout << "Savitsky Golay radius: " << radius << " order: " << order << " -> matrix " << M << 'x' << N << std::endl;
    if ( !savFunc.init( A, 0.F ) )
        std::cout
            << "warning, small values removed from SVD decomposition for radius "
            << radius << " order " << order << std::endl;

    // pre-propagate the solve
    A                        = savFunc.u();
    svdf::col_type        v0 = savFunc.v()[0];
    const svdf::col_type &w  = savFunc.w();
    for ( size_t j = 0; j != N; ++j )
    {
        if ( w[j] )
            v0[j] /= w[j];
        else
            v0[j] = 0.F;
    }

    for ( size_t i = 0; i != M; ++i )
    {
        svdf::col_type &u = A[i];
        for ( size_t j = 0; j != N; ++j )
            u[j] *= v0[j];
    }

    threading::get().dispatch(
        std::bind(
            sav_gol_thread,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3,
            std::ref( r ),
            std::cref( p ),
            std::cref( A ),
            radius ),
        p );

    return r;
}

////////////////////////////////////////

static void nlm_add_contrib_l2(
    float &                   weight,
    float &                   sum,
    const plane &             src,
    int                       x,
    int                       y,
    int                       offX,
    int                       offY,
    int                       window,
    const std::vector<float> &windowKern,
    float                     compareSigma )
{
    float wdiff = 0.F;
    for ( int wY = -window; wY <= window; ++wY )
    {
        float yW    = windowKern[wY + window];
        int   srcY  = y + wY;
        int   testY = offY + wY;
        if ( srcY < src.y1() || srcY > src.y2() )
        {
            // outside source image but from center area so
            // ignore this one
            continue;
        }
        if ( testY < src.y1() || testY > src.y2() )
        {
            // center of pixel is on the image, but the
            // search is off - just return and don't add any
            // of this pixel
            return;
        }
        for ( int wX = -window; wX <= window; ++wX )
        {
            float xW    = windowKern[wY + window];
            int   srcX  = x + wX;
            int   testX = offX + wX;
            if ( srcX < src.x1() || srcX > src.x2() )
            {
                continue;
            }
            if ( testX < src.x1() || testX > src.x2() )
            {
                // center of pixel is on the image, but the
                // search is off - just return and don't add any
                // of this pixel
                return;
            }

            float delta = src.get( srcX, srcY ) - src.get( testX, testY );
            wdiff += delta * delta * xW * yW;
        }
    }
    float finWeight = expf( -wdiff * wdiff * compareSigma );
    sum += src.get( x + offX, y + offY ) * finWeight;
    weight += finWeight;
}

static void standard_nlm_p(
    size_t       tIdx,
    int          s,
    int          e,
    plane &      out,
    const plane &src,
    int          search,
    int          compare,
    float        searchSigma,
    float        compareSigma,
    float        centerWeight )
{
    int                width = out.width();
    std::vector<float> spatKern( compare * 2 + 1 );
    // Buades recommends h = 10*sigma, but his images are 0 - 255 not
    // 0 - 1, still want 10x?
    compareSigma = 1.F / ( compareSigma * compareSigma );
    for ( int y = s; y < e; ++y )
    {
        scanline outscan = scan_ref( out, y );
        for ( int x = 0; x < width; ++x )
        {
            float weight = 0.F;
            float sum    = 0.F;
            for ( int sY = -search; sY <= search; ++sY )
            {
                int curY = y + sY;
                for ( int sX = -search; sX <= search; ++sX )
                {
                    if ( sX == 0 && sY == 0 )
                    {
                        sum += centerWeight * src.get( x + sX, curY );
                        weight += centerWeight;
                        continue;
                    }
                    nlm_add_contrib_l2(
                        weight,
                        sum,
                        src,
                        x,
                        y,
                        sX,
                        sY,
                        compare,
                        spatKern,
                        compareSigma );
                }
            }
            // in a subsequent paper, Buades recommends to average
            // the result w/ a portion of the source image:
            // out = est + max(0, (VarX - sigma^2)/VarX ) * (orig - est)
            //
            // basically saying as the (local) variance of the pixel
            // goes up relative to the global standard deviation
            // provided, mix back in a portion of the original
        }
    }
}

} // namespace

////////////////////////////////////////

namespace image
{
////////////////////////////////////////

plane erode( const plane &p, int radius )
{
    return plane( "p.erode", p.dims(), p, radius );
}

////////////////////////////////////////

plane dilate( const plane &p, int radius )
{
    return plane( "p.dilate", p.dims(), p, radius );
}

////////////////////////////////////////

plane median( const plane &p, int diameter )
{
    if ( diameter == 3 )
    {
        return plane( "p.median_3x3", p.dims(), p );
    }

    return plane( "p.median", p.dims(), p, diameter );
}

////////////////////////////////////////

plane cross_x_img_median( const plane &p )
{
    return plane( "p.cross_x_median", p.dims(), p );
}

////////////////////////////////////////

plane median3( const plane &p1, const plane &p2, const plane &p3 )
{
    return plane( "p.median3", p1.dims(), p1, p2, p3 );
}

////////////////////////////////////////

plane despeckle( const plane &p, float bright, float dark )
{
    return plane( "p.despeckle", p.dims(), p, bright, dark );
}

////////////////////////////////////////

plane bilateral(
    const plane &                        p1,
    const engine::computed_value<int> &  dx,
    const engine::computed_value<int> &  dy,
    const engine::computed_value<float> &sigD,
    const engine::computed_value<float> &sigI )
{
    return plane( "p.bilateral", p1.dims(), p1, dx, dy, sigD, sigI );
}

////////////////////////////////////////

plane cross_bilateral(
    const plane &                        p1,
    const plane &                        ref,
    const engine::computed_value<int> &  dx,
    const engine::computed_value<int> &  dy,
    const engine::computed_value<float> &sigD,
    const engine::computed_value<float> &sigI )
{
    return plane( "p.cross_bilateral", p1.dims(), p1, ref, dx, dy, sigD, sigI );
}

////////////////////////////////////////

plane weighted_bilateral(
    const plane &                        p1,
    const plane &                        w,
    const engine::computed_value<int> &  dx,
    const engine::computed_value<int> &  dy,
    const engine::computed_value<float> &sigD,
    const engine::computed_value<float> &sigI )
{
    return plane(
        "p.weighted_bilateral", p1.dims(), p1, w, dx, dy, sigD, sigI );
}

////////////////////////////////////////

plane wavelet_filter( const plane &p, int levels, float sigma )
{
    precondition( levels > 0, "expect positive number of levels" );
    return wavelet_filter_impl( p, levels, sigma );
}

plane wavelet_filter( const plane &p, int levels, const plane &sigma )
{
    precondition( levels > 0, "expect positive number of levels" );
    precondition(
        p.dims() == sigma.dims(),
        "unable to wavelet_filter planes of different sizes" );
    return wavelet_filter_impl( p, levels, sigma );
}

////////////////////////////////////////

plane guided_filter_mono( const plane &I, const plane &p, int r, float eps )
{
    return guided_filter_impl( I, p, r, eps );
}

plane guided_filter_mono(
    const plane &I, const plane &p, int r, const plane &eps, float epsScale )
{
    precondition(
        p.dims() == eps.dims(),
        "unable to guided_filter planes of different sizes" );
    return guided_filter_impl( I, p, r, eps * epsScale );
}

////////////////////////////////////////

image_buf
guided_filter_mono( const image_buf &I, const image_buf &p, int r, float eps )
{
    image_buf ret = I;
    for ( size_t c = 0; c < ret.size(); ++c )
        ret[c] = guided_filter_mono( I[c], p[c], r, eps );
    return ret;
}

image_buf guided_filter_mono(
    const image_buf &I,
    const image_buf &p,
    int              r,
    const plane &    eps,
    float            epsScale )
{
    image_buf ret = I;
    plane     e   = eps * epsScale;
    for ( size_t c = 0; c < ret.size(); ++c )
        ret[c] = guided_filter_mono( I[c], p[c], r, e );
    return ret;
}

////////////////////////////////////////

image_buf
guided_filter_color( const image_buf &I, const image_buf &p, int r, float eps )
{
    return guided_filter_color_impl( I, p, r, eps );
}

image_buf guided_filter_color(
    const image_buf &I,
    const image_buf &p,
    int              r,
    const plane &    eps,
    float            epsScale )
{
    return guided_filter_color_impl( I, p, r, eps * epsScale );
}

////////////////////////////////////////

plane savitsky_golay_filter( const plane &p, int radius, int order )
{
    return plane( "p.sav_gol", p.dims(), p, radius, order );
}

////////////////////////////////////////

plane savitsky_golay_minimize_error( const plane &p, int radius, int max_order )
{
    plane r;
    plane minErr;
    for ( int o = 0; o <= max_order; ++o )
    {
        plane sg     = savitsky_golay_filter( p, radius, o );
        plane curErr = mse( sg, p, radius );

        if ( o == 0 )
        {
            minErr = curErr;
            r      = sg;
        }
        else
        {
            r      = if_less( curErr, minErr, sg, r );
            minErr = if_less( curErr, minErr, curErr, minErr );
        }
    }

    return r;
}

////////////////////////////////////////

plane nlm(
    const plane &p,
    int          search,
    int          compare,
    float        searchSigma,
    float        compareSigma,
    float        centerWeight )
{
    plane r( p.x1(), p.y1(), p.x2(), p.y2() );

    threading::get().dispatch(
        std::bind(
            standard_nlm_p,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3,
            std::ref( r ),
            std::cref( p ),
            search,
            compare,
            searchSigma,
            compareSigma,
            centerWeight ),
        p );

    return r;
}

////////////////////////////////////////

plane nlm(
    const std::vector<plane> &p,
    int                       search,
    int                       compare,
    float                     searchSigma,
    float                     compareSigma,
    float                     centerWeight );
image_buf
nlm( const image_buf &p,
     int              search,
     int              compare,
     float            searchSigma,
     float            compareSigma,
     float            centerWeight );
image_buf
nlm( const std::vector<image_buf> &p,
     int                           search,
     int                           compare,
     float                         searchSigma,
     float                         compareSigma,
     float                         centerWeight );

/// use L-1 norm instead of L-2 (faster because we can precompute
/// areas for differences)
plane nlm_L1(
    const plane &p,
    int          search,
    int          compare,
    float        searchSigma,
    float        compareSigma,
    float        centerWeight );

////////////////////////////////////////

void add_spatial( engine::registry &r )
{
    using namespace engine;

    r.add(
        op( "p.erode",
            base::choose_runtime( apply_erode ),
            n_scanline_plane_adapter<false, decltype( apply_erode )>(),
            dispatch_scan_processing,
            op::n_to_one ) );
    r.add(
        op( "p.dilate",
            base::choose_runtime( apply_dilate ),
            n_scanline_plane_adapter<false, decltype( apply_dilate )>(),
            dispatch_scan_processing,
            op::n_to_one ) );

    r.add(
        op( "p.median_3x3",
            base::choose_runtime( median_3x3 ),
            n_scanline_plane_adapter<false, decltype( median_3x3 )>(),
            dispatch_scan_processing,
            op::n_to_one ) );

    // rather than recreate a vector every scan, use threading
    r.add( op(
        "p.median", base::choose_runtime( generic_median ), op::threaded ) );

    r.add(
        op( "p.cross_x_median",
            base::choose_runtime( cross_x_median ),
            n_scanline_plane_adapter<false, decltype( cross_x_median )>(),
            dispatch_scan_processing,
            op::n_to_one ) );
    r.add(
        op( "p.median3",
            base::choose_runtime( median_planes ),
            scanline_plane_adapter<true, decltype( median_planes )>(),
            dispatch_scan_processing,
            op::one_to_one ) );

    r.add(
        op( "p.despeckle",
            base::choose_runtime( doDespeckle ),
            n_scanline_plane_adapter<false, decltype( doDespeckle )>(),
            dispatch_scan_processing,
            op::n_to_one ) );

    // wants a temporary scanline for efficiency, so just do generic threading
    r.add(
        op( "p.bilateral",
            base::choose_runtime( apply_bilateral ),
            op::threaded ) );
    r.add(
        op( "p.cross_bilateral",
            base::choose_runtime( apply_cross_bilateral ),
            op::threaded ) );
    r.add(
        op( "p.weighted_bilateral",
            base::choose_runtime( apply_weighted_bilateral ),
            op::threaded ) );

    r.add( op(
        "p.sav_gol", base::choose_runtime( apply_sav_gol ), op::threaded ) );
}

////////////////////////////////////////

} // namespace image
