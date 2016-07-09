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

#include "spatial_filter.h"
#include <base/math_functions.h>
#include <base/contract.h>
#include "plane_ops.h"

////////////////////////////////////////

namespace
{
using namespace image;

static inline std::tuple<plane, plane, plane, plane>
wavelet_decomp( const plane &p, const std::vector<float> &h, const std::vector<float> &g )
{
	plane hhc = convolve_vert( p, h );
	plane ghc = convolve_vert( p, g );
	plane c_j1 = convolve_horiz( hhc, h );
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

inline plane
gaussian( const plane &x, float a, float b, float c )
{
	return a * exp( abs( x - b ) / ( -2.F * c * c ) );
}

inline plane
gaussian( const plane &x, float a, float b, const plane &sigma )
{
	return a * exp( abs( x - b ) / ( -2.F * square( sigma ) ) );
}

template <typename Sigma>
plane
wavelet_filter_impl( const plane &p, size_t levels, Sigma sigma )
{
	precondition( levels > 0, "invalid levels {0}", levels );
	std::vector<std::tuple<plane, plane, plane>> filtLevels;

	std::vector<float> wt_h{ 1.F/16.F, 4.F/16.F, 6.F/16.F, 4.F/16.F, 1.F/16.F };
	std::vector<float> wt_g = base::dirac_negate( wt_h );

	plane c_J = p;
	size_t cnt = levels;
	while ( true )
	{
		auto wd = wavelet_decomp( c_J, wt_h, wt_g );
		c_J = std::get<0>( wd );
		filtLevels.push_back( std::make_tuple( std::get<1>( wd ), std::get<2>( wd ), std::get<3>( wd ) ) );
		if ( cnt == 0 )
			break;

		--cnt;
		wt_h = base::atrous_expand( wt_h );
		wt_g = base::atrous_expand( wt_g );
	}

	postcondition( filtLevels.size() == (levels + 1), "Expecting {0} levels", (levels + 1) );

	float levelScale = 6.F/16.F;
	for ( size_t l = 0; l < levels; ++l )
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
		curPh = curPh * w;
		curPv = curPv * w;
		curPc = curPc * w;

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
	precondition( p.width() == I.width() && p.height() == I.height(), "unable to guided_filter planes of different sizes" );
	plane mean_I = local_mean( I, r );
//	plane mean_II = local_mean( square( I ), r );
//	plane var_I = mean_II - square( mean_I );
	plane var_I = local_variance( I, r );

	plane mean_p = local_mean( p, r );
	plane mean_Ip = local_mean( I * p, r );
	plane cov_Ip = mean_Ip - mean_I * mean_p;

	plane a = cov_Ip / ( var_I + eps );
	plane b = mean_p - a * mean_I;

	plane mean_a = local_mean( a, r );
	plane mean_b = local_mean( b, r );
	return mean_a * I + mean_b;
}


template <typename Epsilon>
inline image_buf
guided_filter_color_impl( const image_buf &I, const image_buf &p, int r, Epsilon eps )
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
		plane var_I_rr = local_mean( I[0] * I[0], r ) - mean_I_r * mean_I_r + eps;
		plane var_I_rg = local_mean( I[0] * I[1], r ) - mean_I_r * mean_I_g;
		plane var_I_rb = local_mean( I[0] * I[2], r ) - mean_I_r * mean_I_b;
		plane var_I_gg = local_mean( I[1] * I[1], r ) - mean_I_g * mean_I_g + eps;
		plane var_I_gb = local_mean( I[1] * I[2], r ) - mean_I_g * mean_I_b;
		plane var_I_bb = local_mean( I[2] * I[2], r ) - mean_I_b * mean_I_b + eps;

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
			plane mean_p = local_mean( p[i], r );
			plane mean_Ip_r = local_mean( I[0] * p[i], r );
			plane mean_Ip_g = local_mean( I[1] * p[i], r );
			plane mean_Ip_b = local_mean( I[2] * p[i], r );
			plane cov_Ip_r = mean_Ip_r - mean_I_r * mean_p;
			plane cov_Ip_g = mean_Ip_g - mean_I_g * mean_p;
			plane cov_Ip_b = mean_Ip_b - mean_I_b * mean_p;
			plane a_r = invrr * cov_Ip_r + invrg * cov_Ip_g + invrb * cov_Ip_b;
			plane a_g = invrg * cov_Ip_r + invgg * cov_Ip_g + invgb * cov_Ip_b;
			plane a_b = invrb * cov_Ip_r + invgb * cov_Ip_g + invbb * cov_Ip_b;
			plane b = mean_p - a_r * mean_I_r - a_g * mean_I_g - a_b * mean_I_b;

			ret[i] = local_mean( a_r, r ) * I[0] + local_mean( a_g, r ) * I[1] + local_mean( a_b, r ) * I[2] + local_mean( b, r );
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

}

////////////////////////////////////////

namespace image
{

////////////////////////////////////////

plane
wavelet_filter( const plane &p, size_t levels, float sigma )
{
	return wavelet_filter_impl( p, levels, sigma );
}

plane
wavelet_filter( const plane &p, size_t levels, const plane &sigma )
{
	precondition( p.width() == sigma.width() && p.height() == sigma.height(), "unable to wavelet_filter planes of different sizes" );
	return wavelet_filter_impl( p, levels, sigma );
}

////////////////////////////////////////

plane
guided_filter_mono( const plane &I, const plane &p, int r, float eps )
{
	return guided_filter_impl( I, p, r, eps );
}

plane
guided_filter_mono( const plane &I, const plane &p, int r, const plane &eps, float epsScale )
{
	precondition( p.width() == eps.width() && p.height() == eps.height(), "unable to guided_filter planes of different sizes" );
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

image_buf
guided_filter_mono( const image_buf &I, const image_buf &p, int r, const plane &eps, float epsScale )
{
	image_buf ret = I;
	plane e = eps * epsScale;
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

image_buf
guided_filter_color( const image_buf &I, const image_buf &p, int r, const plane &eps, float epsScale )
{
	return guided_filter_color_impl( I, p, r, eps * epsScale );
}

////////////////////////////////////////

} // namespace image

