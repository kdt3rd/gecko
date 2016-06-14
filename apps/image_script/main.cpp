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

#include <base/uri.h>
#include <base/scope_guard.h>
#include <base/contract.h>
#include <base/cmd_line.h>
#include <base/cpu_features.h>
#include <base/posix_file_system.h>
#include <media/reader.h>
#include <media/writer.h>
#include <media/sample.h>
#include <image/plane.h>
#include <image/plane_ops.h>
#include <image/media_io.h>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <typeindex>
#include <tuple>

namespace
{

std::vector<float>
atrous_expand( const std::vector<float> &a )
{
	std::vector<float> ret;
	ret.resize( ( a.size() - 1 ) * 2 + 1 );
	for ( size_t i = 0; i != a.size(); ++i )
	{
		ret[i*2] = a[i];
		if ( i+1 != a.size() )
			ret[i*2+1] = 0.F;
	}
//	std::cout << "atrous expand:\n";
//	for ( size_t i = 0; i != ret.size(); ++i )
//		std::cout << i << ": " << ret[i] << std::endl;
	return ret;
}

std::vector<float>
dirac_negate( const std::vector<float> &a )
{
	std::vector<float> ret = a;
	for ( float &v: ret )
		v = -v;
	ret[ret.size()/2] += 1.F;
//	std::cout << "dirac negate:\n";
//	for ( size_t i = 0; i != ret.size(); ++i )
//		std::cout << i << ": " << ret[i] << std::endl;
	return ret;
}

using namespace image;
std::tuple<plane, plane, plane, plane> wavelet_decomp( const plane &p, const std::vector<float> &h, const std::vector<float> &g )
{
	plane hhc = convolve_vert( p, h );
	plane ghc = convolve_vert( p, g );
	plane c_j1 = convolve_horiz( hhc, h );
	plane w1_j1 = convolve_horiz( hhc, g );
	plane w2_j1 = convolve_horiz( ghc, h );
	plane w3_j1 = convolve_horiz( ghc, g );
	return std::make_tuple( c_j1, w1_j1, w2_j1, w3_j1 );
}

plane wavelet_filter( const plane &p, size_t levels, float sigma )
{
	precondition( levels > 0, "invalid levels {0}", levels );
	std::vector<std::tuple<plane, plane, plane>> filtLevels;

	std::vector<float> wt_h{ 1.F/16.F, 4.F/16.F, 6.F/16.F, 4.F/16.F, 1.F/16.F };
	std::vector<float> wt_g = dirac_negate( wt_h );

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
		wt_h = atrous_expand( wt_h );
		wt_g = atrous_expand( wt_g );
	}

	postcondition( filtLevels.size() == (levels + 1), "Expecting {0} levels", (levels + 1) );

	float levelScale = 6.F/16.F;
	for ( size_t l = 0; l < levels; ++l )
	{
		auto &curL = filtLevels[l];
		auto &nextL = filtLevels[l+1];
		plane d_w1 = std::get<0>( curL ) * levelScale - std::get<0>( nextL );
		plane d_w2 = std::get<1>( curL ) * levelScale - std::get<1>( nextL );
		plane d_w3 = std::get<2>( curL ) * levelScale - std::get<2>( nextL );
		std::get<0>( curL ) = std::get<0>( curL ) * exp( clamp( d_w1, 0.F, 1.F ) / ( -2.F * sigma ) );
		std::get<1>( curL ) = std::get<1>( curL ) * exp( clamp( d_w2, 0.F, 1.F ) / ( -2.F * sigma ) );
		std::get<2>( curL ) = std::get<2>( curL ) * exp( clamp( d_w3, 0.F, 1.F ) / ( -2.F * sigma ) );
		sigma *= levelScale;
	}

	plane reconst = c_J;
	for ( auto &l: filtLevels )
		reconst += std::get<0>( l ) + std::get<1>( l ) + std::get<2>( l );

	return reconst;
}

plane replace_high( const plane &p, const plane &filt )
{
	plane lowP = separable_convolve( p, { 0.023F, 0.067F, 0.124F, 0.179F, 0.204F, 0.179F, 0.124F, 0.067F, 0.028F } );
	plane lowF = separable_convolve( filt, { 0.023F, 0.067F, 0.124F, 0.179F, 0.204F, 0.179F, 0.124F, 0.067F, 0.028F } );
	plane highF = filt - lowF;
	return lowP + highF;
}

plane despeckle( const plane &p, float thresh = 0.05F )
{
	plane mid = separable_convolve( p, { 0.25F, 0.5F, 0.25F } );
	plane high = p - mid;
	plane med = cross_x_img_median( p );
	plane highmed = p - med;
	plane highdiff = high - highmed;
	return abs( highdiff ) * 20;
//	plane outhigh = if_less( abs( high ), thresh, high * 0.2, high );
//	plane outhigh = high;

//	return mid + outhigh;
//	return cross_x_img_median( p );
}

int safemain( int argc, char *argv[] )
{
	int spatX = 10;
	int spatY = 10;
	float spatSigmaD = 1.5F;
	float spatSigmaI = 0.01F;
	float varThreshLow = 0.4F;
	float varThreshHigh = 6.0F;
	int temporalRadius = 1;
	base::cmd_line options(
		argv[0],
		base::cmd_line::option(
			0, std::string(),
			"<input_file>", base::cmd_line::arg<1>,
			"Input file pattern to process", true ),
		base::cmd_line::option(
			0, std::string( "variance" ),
			"<file>", base::cmd_line::arg<1>,
			"Input file pattern holding variance", false ),
		base::cmd_line::option(
			0, std::string( "dbgweight" ),
			"file", base::cmd_line::arg<1>,
			"Output file pattern holding derived weight", false ),
		base::cmd_line::option(
			0, std::string( "varthresh" ),
			"<float float>", base::cmd_line::arg<2>,
			"When variance image is between this range, filtering happens (0.4 6)", false ),
		base::cmd_line::option(
			0, std::string( "spatial-weight" ),
			"<float>", base::cmd_line::arg<1>,
			"Specifies the integration sigma", false ),
		base::cmd_line::option(
			0, std::string( "spatial-radius" ),
			"[<size>|<sizeX sizeY>]", base::cmd_line::arg<1,2>,
			"Radius for spatial filtering", false ),
		base::cmd_line::option(
			0, std::string( "spatial-radius-weight" ),
			"<float>", base::cmd_line::arg<1>,
			"Spatial distance weight", false ),
		base::cmd_line::option(
			0, std::string( "temporal-radius" ),
			"<int>", base::cmd_line::arg<1>,
			"specifies radius for temporal filtering", false ),
		base::cmd_line::option(
			0, std::string(),
			"<output_file>", base::cmd_line::arg<1>,
			"Output file pattern to create", true )
	);

	auto errhandler = base::make_guard( [&]() { std::cerr << options << std::endl; } );
	options.parse( argc, argv );
	errhandler.dismiss();

	std::cout << "CPU features:\n";
	base::cpu::output( std::cout );
	std::cout << std::endl;

	auto &spatW = options["spatial-radius"];
	if ( spatW )
	{
		if ( spatW.values().size() == 1 )
			spatX = spatY = atoi( spatW.value() );
		else
		{
			spatX = atoi( spatW.values()[0] );
			spatY = atoi( spatW.values()[1] );
		}
	}
	auto &spatRW = options["spatial-radius-weight"];
	if ( spatRW )
		spatSigmaD = atof( spatRW.value() );
	auto &spatIW = options["spatial-weight"];
	if ( spatIW )
		spatSigmaI = atof( spatIW.value() );
	auto &varT = options["varthresh"];
	if ( varT )
	{
		varThreshLow = atof( varT.values()[0] );
		varThreshHigh = atof( varT.values()[1] );
	}
	auto &tempR = options["temporal-radius"];
	if ( tempR )
		temporalRadius = atoi( tempR.value() );

	media::metadata outputOptions;
	outputOptions["compression"] = std::string( "piz" );

	auto &inP = options["<input_file>"];
	auto &outP = options["<output_file>"];
	auto &varP = options["variance"];
	auto &dbgP = options["dbgweight"];
	if ( inP && outP )
	{
		base::uri inputU( inP.value() );
		if ( ! inputU )
			inputU.set_scheme( "file" );
		base::uri varU;
		if ( varP )
		{
			varU = varP.value();
			if ( ! varU )
				varU.set_scheme( "file" );
		}
		base::uri dbgU;
		if ( dbgP )
		{
			dbgU = dbgP.value();
			if ( ! dbgU )
				dbgU.set_scheme( "file" );
		}
		base::uri outputU( outP.value() );
		if ( ! outputU )
			outputU.set_scheme( "file" );

		media::container c = media::reader::open( inputU );
		std::vector<media::track_description> tds;
		for ( auto &vt: c.video_tracks() )
		{
			std::cout << "Processing track '" << vt->name() << "' frames " << vt->begin() << " - " << vt->end() << " @ rate " << vt->rate() << std::endl;

			media::sample s( vt->begin(), vt->rate() );

			tds.push_back( media::TRACK_VIDEO );
			tds.back().rate( vt->rate() );
			tds.back().offset( vt->begin() );
			tds.back().duration( vt->end() - vt->begin() + 1 );
			tds.back().set_option( "compression", "piz" );
		}

		media::container v;
		if ( varU )
		{
			v = media::reader::open( varU );
			precondition( v.video_tracks().size() == c.video_tracks().size(), "mismatch in video tracks for image and variance image" );
		}
		media::container dbg;
		if ( dbgU )
			dbg = media::writer::open( dbgU, tds, outputOptions );

		media::container oc = media::writer::open( outputU, tds, outputOptions );
		for ( size_t ci = 0; ci != c.video_tracks().size(); ++ci )
		{
			auto &vt = c.video_tracks()[ci];
			for ( int64_t f = vt->begin(); f <= vt->end(); ++f )
			{
				float cnt = 0.F;
				image_buf accumImg;
				for ( int64_t curF = f - temporalRadius; curF <= (f + temporalRadius); ++curF )
				{
					if ( curF < vt->begin() || curF > vt->end() )
						continue;

					media::sample sCur( curF, vt->rate() );
					auto curFrm = sCur( vt );
					image_buf img = extract_frame( *curFrm, { "R", "G", "B" } );
					if ( varU )
					{
						auto curVarFrm = sCur( v.video_tracks()[ci] );
						image_buf varimg = extract_frame( *curVarFrm, { "R", "G", "B" } );
//						plane varP = varimg[0];
//						float varRng = ( varThreshHigh - varThreshLow );
//						for ( int p = 0; p < 3; ++p )
//						{
//							plane prefilt = cross_x_img_median( varP );
//							plane weight = if_less( prefilt, varThreshLow, varP * 0.F + 0.001F, if_greater( prefilt, varThreshHigh, max( varThreshHigh * 1.5 - varP, engine::make_constant(0.F) ) + 0.001F, max( log( varP ), engine::make_constant( 0.F ) ) + 1.F ) );
//							plane normRng = (prefilt - varThreshLow) / varRng;
//							plane prefilt = varimg[2] - varimg[1]; // / sqrt( 1000.F * varimg[1] );
//							plane weight = if_less( prefilt, varThreshLow, prefilt * 0.F, if_greater( prefilt, varThreshHigh, prefilt * 0.F, if_less( normRng, 0.5, normRng, 1.F - normRng ) * 2.F ) );
//							weight = clamp( weight, engine::make_constant( 0.001F ), engine::make_constant( 2.F ) );
//							weight = if_greater( weight, spatSigmaI * 2.F, weight * prefilt, weight + 0.001F );
//							img[p] = prefilt;//weighted_bilateral( img[0], weight, engine::make_constant( spatX ), engine::make_constant( spatY ), engine::make_constant( spatSigmaD ), engine::make_constant( spatSigmaI ) );
//						}

						plane lum = img[0] * 0.3F + img[1] * 0.6F + img[2] * 0.1F;
						lum = log( lum + 1.F );
						plane localvar = local_variance( lum, 5 );
						plane weight = filter_nan( varimg[2], 0.F ) / max( lum, engine::make_constant( 0.00001F ) ) * sqrt( max( localvar, engine::make_constant( 0.00001F ) ) );
						weight = if_less( weight, 0.0001F, clamp( weight, engine::make_constant( 1.F ), engine::make_constant( 1.F ) ), 1.F / weight );
						for ( int p = 0; p < 3; ++p )
							img[p] = replace_high( img[p], exp( weighted_bilateral( log( img[p] + 1.F ), weight, engine::make_constant( spatX ), engine::make_constant( spatY ), engine::make_constant( spatSigmaD ), engine::make_constant( spatSigmaI ) ) ) - 1.F );

//						std::vector<float> wt_h{ 1.F/16.F, 4.F/16.F, 6.F/16.F, 4.F/16.F, 1.F/16.F };
//						std::vector<float> wt_g = dirac_negate( wt_h );
//
//						auto wd1 = wavelet_decomp( lum, wt_h, wt_g );
//						std::vector<float> wt2_h = atrous_expand( wt_h );
//						std::vector<float> wt2_g = atrous_expand( wt_g );
//						auto wd2 = wavelet_decomp( std::get<0>( wd1 ), wt2_h, wt2_g );
//						plane reconst = std::get<0>( wd2 ) + std::get<1>( wd2 ) + std::get<2>( wd2 ) + std::get<3>( wd2 ) + std::get<1>( wd1 ) + std::get<2>( wd1 ) + std::get<3>( wd1 );
//						img[0] = std::get<1>( wd1 ) + std::get<2>( wd1 ) + std::get<3>( wd1 );
//						img[1] = std::get<1>( wd2 ) + std::get<2>( wd2 ) + std::get<3>( wd2 );
//						img[2] = reconst;
//						auto wd = wavelet_decomp( lum, , { -1.F/16.F, -4.F/16.F, 10.F/16.F, -4.F/16.F, -1.F/16.F } );
//						img[0] = wd.first;
//						img[1] = wd.second;
//						img[2] = wd.first + wd.second;
#if 0
						if ( dbgU )
						{
							image_buf dbgImg;
							dbgImg.add_plane( weight );
							dbgImg.add_plane( localvar );
							dbgImg.add_plane( lum );
							dbg.video_tracks()[ci]->store( f, to_frame( dbgImg, { "R", "G", "B" }, "f16" ) );
						}
#endif
					}
					else
					{
						for ( int p = 0; p < 3; ++p )
							img[p] = wavelet_filter( img[p], 2, 0.02 );
//							img[p] = bilateral( img[p], engine::make_constant( spatX ), engine::make_constant( spatY ), engine::make_constant( spatSigmaD ), engine::make_constant( spatSigmaI ) );
					}

					if ( cnt < 1.F )
						accumImg = img;
					else
					{
						for ( int p = 0; p < 3; ++p )
							accumImg[p] += img[p];
					}
					cnt += 1.F;
				}
				for ( int p = 0; p < 3; ++p )
					accumImg[p] /= cnt;

//				accumImg[0].graph_ptr()->optimize();
//				accumImg[0].graph_ptr()->dump_dot( "plane0.dot" );
//				accumImg[1].graph_ptr()->dump_dot( "plane1.dot" );
//				accumImg[2].graph_ptr()->dump_dot( "plane2.dot" );
//				accumImg[0].graph_ptr()->dump_refs( std::cout );
				oc.video_tracks()[ci]->store( f, to_frame( accumImg, { "R", "G", "B" }, "f16" ) );
				std::cout << "Finished frame: " << f << std::endl;
			}
		}
	}
	image::allocator::get().report( std::cout );

	return 0;
}

}

int main( int argc, char *argv[] )
{
	try
	{
		return safemain( argc, argv );
	}
	catch ( const std::exception &e )
	{
		base::print_exception( std::cerr, e );
	}
}
