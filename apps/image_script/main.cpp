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
#include <fstream>
#include <typeindex>

namespace
{

using namespace image;

int safemain( int argc, char *argv[] )
{
	std::cout << "CPU features:\n";
	base::cpu::output( std::cout );
	std::cout << "\nStarting graph generation..." << std::endl;

	auto avev = sum( test_refcount() ) / static_cast<double>( 1920 * 1080 );
//	std::cout << "sum created: id " << avev.id() << std::endl;

	// should only be one reference left at this point
//	avev.graph_ptr()->dump_refs( std::cout );
	avev.graph_ptr()->dump_dot( "graphOrig.dot" );

//	avev.graph_ptr()->clean_graph();
//	avev.graph_ptr()->dump_dot( "graphClean.dot" );

	avev.graph_ptr()->optimize();
	avev.graph_ptr()->dump_dot( "graphOpt.dot" );
	
	// force the graph to be processed
	std::cout << "Initiating processing..." << std::endl;
	double ave = static_cast<double>( avev );
	std::cout << "Processing finished!" << std::endl;
	std::cout << "ave: " << ave << std::endl;

//	avev.graph_ptr()->clean_graph();
//	avev.graph_ptr()->dump_dot( "graphClean.dot" );

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
			std::cout << "Processing track '" << vt->name() << "' frames " << vt->begin() << " - " << vt->end() << " @ rate " << vt->rate() << std::endl;

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

				image_buf img = extract_frame( *curFrm, { "R", "G", "B" } );

				for ( int c = 0; c < 3; ++c )
					img[c] *= 0.5F;

				oc.video_tracks()[ovt]->store( f, to_frame( img, { "R", "G", "B" }, "f16" ) );

				std::cout << "wrote " << s.offset() << std::endl;
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
