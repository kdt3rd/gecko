//
// Copyright (c) 2016-2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include <base/uri.h>
#include <base/scope_guard.h>
#include <base/contract.h>
#include <base/cmd_line.h>
#include <base/posix_file_system.h>
#include <media/reader.h>
#include <media/writer.h>
#include <media/sample.h>
#include <image/plane_ops.h>
#include <image/media_io.h>
#include <image/threading.h>
#include <sstream>
#include <iostream>
#include <typeindex>

namespace
{

int safemain( int argc, char *argv[] )
{
	base::cmd_line options(
		argv[0],
		base::cmd_line::option(
			0, std::string( "output-settings" ),
			"<string>", base::cmd_line::arg<1>,
			"Comma separated name=value setting for output file format options", false ),
		base::cmd_line::option(
			0, std::string(),
			"<input_file>", base::cmd_line::arg<1>,
			"Input file pattern to transcode", true ),
		base::cmd_line::option(
			0, std::string(),
			"<output_file>", base::cmd_line::arg<1>,
			"Output file pattern to create", true )
	);

	auto errhandler = base::make_guard( [&]() { std::cerr << options << std::endl; } );
	options.parse( argc, argv );
	errhandler.dismiss();

	auto &inP = options["<input_file>"];
	auto &outP = options["<output_file>"];
	if ( inP && outP )
	{
		base::uri inputU( inP.value() );
		if ( ! inputU )
			inputU.set_scheme( "file" );
		base::uri outputU( outP.value() );
		if ( ! outputU )
			outputU.set_scheme( "file" );

		media::container c = media::reader::open( inputU );
		std::vector<media::track_description> tds;
		for ( auto &vt: c.video_tracks() )
		{
			std::cout << "Converting track '" << vt->name() << "' frames " << vt->begin() << " - " << vt->end() << " @ rate " << vt->rate() << std::endl;

			media::sample s( vt->begin(), vt->rate() );

			tds.push_back( media::TRACK_VIDEO );
			tds.back().rate( vt->rate() );
			tds.back().offset( vt->begin() );
			tds.back().duration( vt->end() - vt->begin() + 1 );
		}

		std::string outOpts;
		auto &outParams = options["output-settings"];
		if ( outParams )
			outOpts = outParams.value();
		media::parameter_set outputOptions = media::writer::parameters_by_ext( outputU, outOpts );

		media::container oc = media::writer::open( outputU, tds, outputOptions );
		size_t ovt = 0;
		for ( auto &vt: c.video_tracks() )
		{
			for ( int64_t f = vt->begin(); f <= vt->end(); ++f )
			{
				media::sample s( f, vt->rate() );

				auto curFrm = s( vt );

				using namespace image;
				image_buf img = extract_frame( *curFrm, std::string(), std::string(), { "R", "G", "B" } );
				int origw = img.width();
				int origh = img.height();
				int neww = origw / 2;
				int newh = origh / 2;
				for ( size_t p = 0; p != img.size(); ++p )
				{
					img[p] = resize_bicubic( img[p], neww, newh );
//					plane tmpBil = resize_bilinear( img[p], neww, newh );
//					plane tmpBic = resize_bicubic( img[p], neww, newh );
//					plane tmpPt = resize_point( img[p], neww, newh );
//					plane origSzBic = resize_bicubic( tmpBic, origw, origh );
//					plane origSzBil = resize_bilinear( tmpBil, origw, origh );
//					plane origSzPt = resize_point( tmpPt, origw, origh );
//					img[p] = ( origSzBic - origSzPt ) * 100;
//					img[p] = origSzBic;
//					img[p] = origSzBil;
//					img[p] = origSzPt;
				}
				

				oc.video_tracks()[ovt]->store( f, to_frame( img, { "R", "G", "B" }, "f16" ), allocator::get() );
//				oc.video_tracks()[ovt]->store( f, curFrm );
			}
			++ovt;
		}
	}

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
