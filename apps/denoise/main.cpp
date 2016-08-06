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
#include <image/threading.h>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <typeindex>
#include <tuple>

namespace
{
using namespace image;

//plane replace_high( const plane &p, const plane &filt )
//{
//	plane lowP = separable_convolve( p, { 0.023F, 0.067F, 0.124F, 0.179F, 0.204F, 0.179F, 0.124F, 0.067F, 0.028F } );
//	plane lowF = separable_convolve( filt, { 0.023F, 0.067F, 0.124F, 0.179F, 0.204F, 0.179F, 0.124F, 0.067F, 0.028F } );
//	plane highF = filt - lowF;
//	return lowP + highF;
//}

int safemain( int argc, char *argv[] )
{
	int spatX = 7;
	int spatY = -1;
	int waveletLevels = 1;
	float spatSigmaD = 1.5F;
	float spatSigmaI = 0.025F;
	float mseThresh = 0.0075F;
	int mseRadius = 5;
	int matchRadius = 4;
	int tempIters = 5;
	int temporalRadius = 0;
	bool estimateNoise = false;
	bool useLog = false;
	std::string spatmethod = "guided_color";
	std::string temporalmethod;
	std::string integmethod = "mse";
	int64_t frameStart = std::numeric_limits<int64_t>::min();
	int64_t frameEnd = std::numeric_limits<int64_t>::min();
	base::cmd_line options(
		argv[0],
		base::cmd_line::option(
			0, std::string(),
			"<input_file>", base::cmd_line::arg<1>,
			"Input file pattern to process", true ),
		base::cmd_line::option(
			0, std::string( "variance" ),
			"<file>", base::cmd_line::arg<1>,
			"Input file pattern holding variance to compute weight", false ),
		base::cmd_line::option(
			0, std::string( "estimate-noise" ),
			std::string(), base::cmd_line::flag,
			"Runs a routine to estimate noise in a local area", false ),
		base::cmd_line::option(
			0, std::string( "use-log" ),
			std::string(), base::cmd_line::flag,
			"Processes the image in log space instead of linear", false ),
		base::cmd_line::option(
			0, std::string( "spatial-method" ),
			"<guided_color|guided_mono|wavelet|bilateral|despeckle|none>", base::cmd_line::arg<1>,
			"Specifies the spatial method used", false ),
		base::cmd_line::option(
			0, std::string( "temporal-method" ),
			"<patchmatch|hierpatch>", base::cmd_line::arg<1>,
			"Specifies the temporal method used to align frames", false ),
		base::cmd_line::option(
			0, std::string( "integration-method" ),
			"<mse|robustave|dumb>", base::cmd_line::arg<1>,
			"Specifies the integration method used to integrate frames", false ),
		base::cmd_line::option(
			0, std::string( "spatial-weight" ),
			"<float>", base::cmd_line::arg<1>,
			"Specifies the integration sigma", false ),
		base::cmd_line::option(
			0, std::string( "mse-thresh" ),
			"<float>", base::cmd_line::arg<1>,
			"Specifies the integration error threshold", false ),
		base::cmd_line::option(
			0, std::string( "mse-radius" ),
			"<int>", base::cmd_line::arg<1>,
			"Specifies the radius used to find the potential integration error", false ),
		base::cmd_line::option(
			0, std::string( "match-radius" ),
			"<int>", base::cmd_line::arg<1>,
			"Specifies the radius used to find temporal matches for relevant algorithms", false ),
		base::cmd_line::option(
			0, std::string( "temporal-iters" ),
			"<int>", base::cmd_line::arg<1>,
			"Specifies the iteration count used to find temporal matches for relevant algorithms", false ),
		base::cmd_line::option(
			0, std::string( "spatial-radius" ),
			"[<size>|<sizeX sizeY>]", base::cmd_line::arg<1,2>,
			"Radius for spatial filtering", false ),
		base::cmd_line::option(
			0, std::string( "spatial-radius-weight" ),
			"<float>", base::cmd_line::arg<1>,
			"Spatial distance weight", false ),
		base::cmd_line::option(
			0, std::string( "wavelet-levels" ),
			"<int>", base::cmd_line::arg<1>,
			"for wavelet filter, how many levels to apply", false ),
		base::cmd_line::option(
			0, std::string( "temporal-radius" ),
			"<int>", base::cmd_line::arg<1>,
			"specifies radius for temporal filtering", false ),
		base::cmd_line::option(
			'f', std::string( "frames" ),
			"[<frame>|<start end>]", base::cmd_line::arg<1,2>,
			"Frame range to process", false ),
		base::cmd_line::option(
			'T', std::string( "threads" ),
			"<int>", base::cmd_line::arg<1>,
			"Number of threads to use for processing", false ),
		base::cmd_line::option(
			0, std::string(),
			"<output_file>", base::cmd_line::arg<1>,
			"Output file pattern to create", true )
	);
	options.add_help();

	auto errhandler = base::make_guard( [&]() { std::cerr << options << std::endl; } );
	options.parse( argc, argv );
	errhandler.dismiss();

	auto &threads = options["threads"];
	if ( threads )
	{
		int tCount = atoi( threads.value() );
		threading::init( tCount );
	}

	std::cout << "CPU features:\n";
	base::cpu::output( std::cout );
	std::cout << std::endl;

	auto &spatM = options["spatial-method"];
	if ( spatM )
	{
		std::string m = spatM.value();
		if ( m == "guided_mono" )
			spatmethod = m;
		else if ( m == "guided_color" )
			spatmethod = m;
		else if ( m == "bilateral" )
			spatmethod = m;
		else if ( m == "wavelet" )
			spatmethod = m;
		else if ( m == "despeckle" )
			spatmethod = m;
		else if ( m == "none" )
			spatmethod = m;
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

	auto &waveL = options["wavelet-levels"];
	if ( waveL )
	{
		waveletLevels = atoi( spatW.value() );
		if ( waveletLevels <= 0 )
			throw_runtime( "wavelet levels must be a positive integer (1-N), got {0}", spatW.value() );
	}

	auto &tempMethod = options["temporal-method"];
	if ( tempMethod )
	{
		std::string m = tempMethod.value();
		if ( m == "patchmatch" )
			temporalmethod = m;
		else if ( m == "hierpatch" )
			temporalmethod = m;
		else
			throw_runtime( "Invalid temporal method requested: {0}", m );
	}

	auto &tempR = options["temporal-radius"];
	if ( tempR )
		temporalRadius = atoi( tempR.value() );
	if ( temporalRadius < 0 )
		throw_runtime( "Invalid temporal radius specified {0}, must be 0 or positive integer", temporalRadius );
	if ( temporalRadius > 0 && temporalmethod.empty() )
		throw_runtime( "Please specify a temporal method to use if specifying a temporal radius" );

	auto &combMethod = options["integration-method"];
	if ( combMethod )
	{
		std::string m = combMethod.value();
		if ( m == "mse" )
			integmethod = m;
		else if ( m == "robustave" )
			integmethod = m;
		else if ( m == "dumb" )
			integmethod = m;
		else
			throw_runtime( "Invalid temporal method requested: {0}", m );
	}

	auto &mseV = options["mse-thresh"];
	if ( mseV )
	{
		if ( integmethod != "mse" )
			throw_runtime( "MSE threshold only used when integration method is mse" );

		mseThresh = atof( mseV.value() );
	}

	auto &mseR = options["mse-radius"];
	if ( mseR )
	{
		if ( integmethod != "mse" )
			throw_runtime( "MSE threshold only used when integration method is mse" );

		mseRadius = atoi( mseR.value() );
	}

	auto &matchR = options["match-radius"];
	if ( matchR )
		matchRadius = atoi( matchR.value() );

	auto &tempIt = options["temporal-iters"];
	if ( tempIt )
		tempIters = atoi( tempIt.value() );

	estimateNoise = static_cast<bool>( options["estimate-noise"] );
	useLog = static_cast<bool>( options["use-log"] );
	if ( useLog )
		std::cout << "Using log of image where appropriate" << std::endl;

	media::metadata outputOptions;
	outputOptions["compression"] = std::string( "piz" );
	auto &frames = options["frames"];
	if ( frames )
	{
		if ( frames.values().size() == 1 )
			frameStart = frameEnd = atoll( frames.value() );
		else
		{
			frameStart = atoll( frames.values()[0] );
			frameEnd = atoll( frames.values()[1] );
		}
		if ( frameStart > frameEnd )
			throw_runtime( "Invalid frame range {0} - {1} entered: start past end", frameStart, frameEnd );
	}

	auto &inP = options["<input_file>"];
	auto &outP = options["<output_file>"];
	auto &varP = options["variance"];
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
		base::uri outputU( outP.value() );
		if ( ! outputU )
			outputU.set_scheme( "file" );

		media::container c = media::reader::open( inputU );
		std::vector<media::track_description> tds;
		for ( auto &vt: c.video_tracks() )
		{
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
			std::cout << "Opening variance image " << varU << std::endl;
			v = media::reader::open( varU );
			precondition( v.video_tracks().size() == c.video_tracks().size(), "mismatch in video tracks for image and variance image" );
		}

		media::container oc = media::writer::open( outputU, tds, outputOptions );
		for ( size_t ci = 0; ci != c.video_tracks().size(); ++ci )
		{
			auto &vt = c.video_tracks()[ci];
			int64_t fs = vt->begin();
			int64_t fe = vt->end();
			if ( frameStart != std::numeric_limits<int64_t>::min() )
				fs = frameStart;
			if ( frameEnd != std::numeric_limits<int64_t>::min() )
				fe = frameEnd;

			std::cout << "Processing track '" << vt->name() << "' of '" << inputU.pretty() << "': frames " << fs << " - " << fe << " of " << vt->begin() << " - " << vt->end() << " @ rate " << vt->rate() << std::endl;


			for ( int64_t f = fs; f <= fe; ++f )
			{
				std::cout << "Processing frame: " << f << std::endl;
				image_buf centerImg;
				image_buf weight;
				{
					media::sample cenSamp( f, vt->rate() );
					auto centerFrm = cenSamp( vt );
					centerImg = extract_frame( *centerFrm, { "R", "G", "B" } );

					if ( varU )
					{
						auto curVarFrm = cenSamp( v.video_tracks()[ci] );
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

						plane lum = centerImg[0] * 0.3F + centerImg[1] * 0.6F + centerImg[2] * 0.1F;
//						lum = log( lum + 1.F );

						plane localvar = local_variance( lum, 5 );
						plane vweight = filter_nan( varimg[2], 0.F ) / max( lum, engine::make_constant( 0.00001F ) ) * sqrt( max( localvar, engine::make_constant( 0.00001F ) ) );
						vweight = if_less( vweight, 0.0001F, clamp( vweight, engine::make_constant( 1.F ), engine::make_constant( 1.F ) ), 1.F / vweight );

						weight = centerImg;
						for ( auto &p: weight )
							p = vweight;
					}
					else if ( estimateNoise )
					{
						std::cout << "TODO: Insert finised noise estimate" << std::endl;
					}
				}

				image_buf filteredCenter = centerImg;
				if ( useLog )
				{
					for ( int p = 0; p < 3; ++p )
						filteredCenter[p] = log1p( filteredCenter[p] );
				}

				if ( spatmethod == "guided_color" )
				{
					if ( weight.empty() )
						filteredCenter = guided_filter_color( filteredCenter, filteredCenter, std::max( spatX, spatY ), spatSigmaI * spatSigmaI );
					else
					{
						plane lumw = weight[0] * 0.3F + weight[1] * 0.6F + weight[2] * 0.1F;
						filteredCenter = guided_filter_color( filteredCenter, filteredCenter, std::max( spatX, spatY ), lumw * spatSigmaI * spatSigmaI );
					}
				}
				else if ( spatmethod == "guided_mono" )
				{
					for ( int p = 0; p < 3; ++p )
					{
						if ( weight.empty() )
							filteredCenter[p] = guided_filter_mono( filteredCenter[p], filteredCenter[p], std::max( spatX, spatY ), spatSigmaI * spatSigmaI );
						else
							filteredCenter[p] = guided_filter_mono( filteredCenter[p], filteredCenter[p], std::max( spatX, spatY ), weight[p] * spatSigmaI * spatSigmaI );
					}
				}
				else if ( spatmethod == "bilateral" )
				{
					for ( int p = 0; p < 3; ++p )
					{
						if ( weight.empty() )
							filteredCenter[p] = bilateral( filteredCenter[p], engine::make_constant( spatX ), engine::make_constant( spatY ), engine::make_constant( spatSigmaD ), engine::make_constant( spatSigmaI ) );
						else
							filteredCenter[p] = weighted_bilateral( filteredCenter[p], weight[p], engine::make_constant( spatX ), engine::make_constant( spatY ), engine::make_constant( spatSigmaD ), engine::make_constant( spatSigmaI ) );
					}
				}
				else if ( spatmethod == "wavelet" )
				{
					for ( int p = 0; p < 3; ++p )
					{
						if ( weight.empty() )
							filteredCenter[p] = wavelet_filter( filteredCenter[p], waveletLevels, spatSigmaI );
						else
							filteredCenter[p] = wavelet_filter( filteredCenter[p], waveletLevels, weight[p] * spatSigmaI );
					}
				}
				else if ( spatmethod == "despeckle" )
				{
					for ( int p = 0; p < 3; ++p )
						filteredCenter[p] = despeckle( filteredCenter[p], spatSigmaI );
				}

				if ( useLog )
				{
					for ( int p = 0; p < 3; ++p )
						filteredCenter[p] = expm1( filteredCenter[p] );
				}

//				debug_save_image( filteredCenter, "filtered_center.#######.exr", f, { "R", "G", "B" }, "f16" );
				if ( temporalRadius <= 0 )
				{
					oc.video_tracks()[ci]->store( f, to_frame( filteredCenter, { "R", "G", "B" }, "f16" ) );
					std::cout << "Finished frame: " << f << std::endl;
					continue;
				}

				float cnt = 0.F;
				image_buf accumImg;
//				plane integAmt;
				for ( int64_t curF = f - temporalRadius; curF <= (f + temporalRadius); ++curF )
				{
					if ( curF < vt->begin() || curF > vt->end() )
						continue;

					media::sample sCur( curF, vt->rate() );

					image_buf img;
					if ( curF == f )
						img = centerImg;
					else
					{
						std::cout << "  reading temporal frame " << curF << std::endl;
						auto curFrm = sCur( vt );
						img = extract_frame( *curFrm, { "R", "G", "B" } );

//						plane lumA = centerImg[0] * 0.3F + centerImg[1] * 0.6F + centerImg[2] * 0.1F;
//						plane lumB = img[0] * 0.3F + img[1] * 0.6F + img[2] * 0.1F;
//						vector_field vf = patch_match( log1p( lumA ), log1p( lumB ), f, curF, 3, patch_style::SSD_GRAD, 16 );
						image_buf tmpCen = centerImg;
						image_buf tmpImg = img;
						if ( useLog )
						{
							for ( int i = 0; i < 3; ++i )
							{
								tmpCen[i] = log1p( centerImg[i] );
								tmpImg[i] = log1p( img[i] );
							}
						}

						if ( temporalmethod == "patchmatch" )
						{
							// TODO: add parameters for the parameters
							vector_field vf = patch_match( tmpCen, tmpImg, f, curF, matchRadius, patch_style::SSD, tempIters );
//							std::stringstream fnb;
//							int offset = f - curF;
//							fnb << "vec_field_" << f << '_' << (offset < 0 ?'m':'p') << std::abs(offset) << ".exr";
//							debug_save_image( colorize( vf, true ), fnb.str(), f, { "R", "G", "B", "A" }, "f16" );
							img[0] = warp_dirac( img[0], vf, true );
							img[1] = warp_dirac( img[1], vf, true );
							img[2] = warp_dirac( img[2], vf, true );
//							std::stringstream warpfn;
//							warpfn << "warped_" << f << '_' << (offset < 0 ?'m':'p') << std::abs(offset) << ".exr";
//							debug_save_image( img, warpfn.str(), f, { "R", "G", "B" }, "f16" );
						}
						else if ( temporalmethod == "hierpatch" )
						{
							// TODO: add parameters for the parameters
							vector_field vf = hier_patch_match( tmpCen, tmpImg, f, curF, matchRadius, patch_style::SSD, tempIters );
//							std::stringstream fnb;
//							int offset = f - curF;
//							fnb << "vec_field_" << f << '_' << (offset < 0 ?'m':'p') << std::abs(offset) << ".exr";
//							debug_save_image( colorize( vf, true ), fnb.str(), f, { "R", "G", "B", "A" }, "f16" );
							img[0] = warp_dirac( img[0], vf, true );
							img[1] = warp_dirac( img[1], vf, true );
							img[2] = warp_dirac( img[2], vf, true );
//							std::stringstream warpfn;
//							warpfn << "warped_" << f << '_' << (offset < 0 ?'m':'p') << std::abs(offset) << ".exr";
//							debug_save_image( img, warpfn.str(), f, { "R", "G", "B" }, "f16" );
						}

						// TODO: add integration logic here
						if ( integmethod == "mse" )
						{
							image_buf tmpI;
							for ( int i = 0; i < 3; ++i )
							{
								plane e = mse( img[i], centerImg[i], mseRadius );
								tmpI.add_plane( e * 100.F );
								plane intmap = threshold( e, mseThresh );
//								if ( ! integAmt.valid() )
//									integAmt = intmap;
//								else
//									integAmt += intmap;
								// TODO: blur integration map and blend instead of hard thresh
								img[i] = if_less( e, mseThresh, img[i], filteredCenter[i] );
							}
//							int offset = f - curF;
//							std::stringstream errfn;
//							errfn << "mse_" << f << '_' << (offset < 0 ?'m':'p') << std::abs(offset) << ".exr";
//							debug_save_image( tmpI, errfn.str(), f, { "R", "G", "B" }, "f16" );
						}
						else if ( integmethod == "robustave" )
						{
							throw_not_yet();
						}
					}

					if ( cnt < 1.F )
						accumImg = img;
					else
					{
						for ( int p = 0; p < 3; ++p )
							accumImg[p] += img[p];
					}
					cnt += 1.F;
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
//				if ( ! integAmt.valid() )
				{
					oc.video_tracks()[ci]->store( f, to_frame( accumImg, { "R", "G", "B" }, "f16" ) );
				}
//				else
//				{
//					accumImg.add_plane( integAmt / ( ( cnt - 1.F ) * 3.F ) );
//					oc.video_tracks()[ci]->store( f, to_frame( accumImg, { "R", "G", "B", "A" }, "f16" ) );
//				}
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
