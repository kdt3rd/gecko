// Copyright (c) 2016-2017 Kimball Thurston
// SPDX-License-Identifier: MIT

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

int safemain( int argc, char *argv[] )
{
	base::cmd_line options(
		argv[0],
		base::cmd_line::option(
			0, std::string(),
			"<input_script>", base::cmd_line::arg<1>,
			"Input script to process", true )
	);

	auto errhandler = base::make_guard( [&]() { std::cerr << options << std::endl; } );
	options.parse( argc, argv );
	errhandler.dismiss();

	std::cout << "CPU features:\n";
	base::cpu::output( std::cout );
	std::cout << std::endl;

	auto &inP = options["<input_script>"];
	if ( inP )
	{
		base::uri inputU( inP.value() );
		if ( ! inputU )
			inputU.set_scheme( "file" );

		std::cout << "Processing script: " << inputU.pretty() << std::endl;

		std::cout << "Add lua parse layer!!!!" << std::endl;
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
