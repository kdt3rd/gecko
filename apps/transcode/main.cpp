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

	media::metadata outputOptions;
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
			tds.back().set_option( "compression", "piz" );
		}

		media::container oc = media::writer::open( outputU, tds, outputOptions );
		size_t ovt = 0;
		for ( auto &vt: c.video_tracks() )
		{
			for ( int64_t f = vt->begin(); f <= vt->end(); ++f )
			{
				media::sample s( f, vt->rate() );
			
				auto curFrm = s( vt );
				if ( f == vt->begin() )
					std::cout << " size: " << curFrm->width() << " x " << curFrm->height() << ", " << curFrm->size() << " channels" << std::endl;

				oc.video_tracks()[ovt]->store( f, curFrm );
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
