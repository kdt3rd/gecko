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
using namespace image;
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
	int spatX = 7;
	int spatY = -1;
	float spatSigmaD = 1.5F;
	float spatSigmaI = 0.025F;
	float varThreshLow = 0.4F;
	float varThreshHigh = 6.0F;
	int temporalRadius = 0;
	std::string method = "guided_color";

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
			0, std::string( "spatial-method" ),
			"<guided_color|guided_mono|bilateral>", base::cmd_line::arg<1>,
			"Specifies the spatial method used", false ),
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

	auto &spatMethod = options["spatial-method"];
	if ( spatMethod )
	{
		std::string m = spatMethod.value();
		if ( m == "guided_mono" )
			method = m;
		else if ( m == "guided_color" )
			method = m;
		else if ( m == "bilateral" )
			method = m;
		else
			throw_runtime( "Invalid spatial method requested: {0}", m );
	}

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
	if ( spatY < 0 )
		spatY = spatX;

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
//						auto curVarFrm = sCur( v.video_tracks()[ci] );
//						image_buf varimg = extract_frame( *curVarFrm, { "R", "G", "B" } );
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
//						lum = log( lum + 1.F );
#if 0
						plane localvar = local_variance( lum, 5 );
						plane weight = filter_nan( varimg[2], 0.F ) / max( lum, engine::make_constant( 0.00001F ) ) * sqrt( max( localvar, engine::make_constant( 0.00001F ) ) );
						weight = if_less( weight, 0.0001F, clamp( weight, engine::make_constant( 1.F ), engine::make_constant( 1.F ) ), 1.F / weight );
						for ( int p = 0; p < 3; ++p )
							img[p] = replace_high( img[p], exp( weighted_bilateral( log( img[p] + 1.F ), weight, engine::make_constant( spatX ), engine::make_constant( spatY ), engine::make_constant( spatSigmaD ), engine::make_constant( spatSigmaI ) ) ) - 1.F );
#endif

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
						image_buf fimg;

						if ( method == "guided_color" )
						{
							fimg = guided_filter_color( img, img, std::max( spatX, spatY ), spatSigmaI * spatSigmaI );
						}
						else if ( method == "guided_mono" )
						{
							fimg = img;
							for ( int p = 0; p < 3; ++p )
								fimg[p] = guided_filter_mono( img[p], img[p], std::max( spatX, spatY ), spatSigmaI * spatSigmaI );
						}
						else if ( method == "bilateral" )
						{
							fimg = img;
							for ( int p = 0; p < 3; ++p )
								fimg[p] = bilateral( img[p], engine::make_constant( spatX ), engine::make_constant( spatY ), engine::make_constant( spatSigmaD ), engine::make_constant( spatSigmaI ) );
						}

						img = fimg;
#if 0
						const float sigma2 = 0.00005;

						plane detailmask;
						for ( int p = 0; p < 3; ++p )
						{
							plane detail = img[p] - fimg[p];
							plane noisemask = threshold( exp( square( detail ) / ( -2.F * sigma2 ) ), 0.5F );
							if ( p == 0 )
								detailmask = noisemask;
							else
								detailmask = max( detailmask, noisemask );
						}
						for ( int p = 0; p < 3; ++p )
						{
							plane detail = img[p] - fimg[p];
							img[p] = img[p] - detail * detailmask;
						}
#endif
						
//							img[p] = guided_filter_mono( img[p], img[p], 5, scale * .0005F );
//							img[p] = guided_filter_mono( img[p], img[p], 11, lum * 0.0005F );
//							img[p] = wavelet_filter( img[p], 1, 0.1F );
//							img[p] = 
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
				if ( cnt > 1.F )
				{
					for ( int p = 0; p < 3; ++p )
						accumImg[p] /= cnt;
				}

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